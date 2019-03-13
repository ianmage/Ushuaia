#include "frameBuffer.h"
#include "viewState.h"
#include <cassert>


namespace Ushuaia
{

FrameBuffer const * FrameBuffer::s_currFB = nullptr;
FrameBuffer const * FrameBuffer::s_backBuf = nullptr;
decltype(FrameBuffer::s_rts) FrameBuffer::s_rts;
decltype(FrameBuffer::s_viewChannelID) FrameBuffer::s_viewChannelID = 0xFF;
static Matrix4x4 s_mtxOrtho;


bool FrameBuffer::Init()
{
	bgfx::Caps const * caps = bgfx::getCaps();
	bx::mtxOrtho(s_mtxOrtho.v, 0.f, 1.f, 1.f, 0.f, 0.f, 100.f, 0.f, caps->homogeneousDepth);
	assert(!s_backBuf);
	s_backBuf = new FrameBuffer(g_viewState.width, g_viewState.height, bgfx::TextureFormat::Unknown,
		BGFX_TEXTURE_RT | BGFX_TEXTURE_RT_WRITE_ONLY);
	s_currFB = nullptr;

	return true;
}


void FrameBuffer::Fini()
{
	delete s_backBuf;
	s_backBuf = nullptr;

	s_currFB = nullptr;

	for (auto pFB : s_rts)
		delete pFB;
	s_rts.clear();
}


FrameBuffer::FrameBuffer(uint16_t w, uint16_t h, bgfx::TextureFormat::Enum fmt, uint64_t flags, uint8_t mipCnt)
: handle_(BGFX_INVALID_HANDLE)
, width_(w), height_(h)
{
#ifdef _DEBUG
	assert(w * h > 0 && w <= 4096 && h <= 4096);
#endif
	if (bgfx::TextureFormat::Unknown == fmt || bgfx::TextureFormat::UnknownDepth == fmt)
		return;

	bgfx::TextureInfo texInfo;
	texInfo.width = w;
	texInfo.height = h;
	texInfo.depth = 1;
	texInfo.numLayers = 1;
	texInfo.numMips = mipCnt;
	texInfo.format = fmt;
	texInfo.cubeMap = false;

	rTexs_.emplace_back(texInfo, flags);

	Reset();
}


FrameBuffer::FrameBuffer(bgfx::TextureInfo const * texInfos, uint8_t numRT)
: handle_(BGFX_INVALID_HANDLE)
{
	assert(numRT > 0 && numRT <= 8);
	width_ = texInfos[0].width;
	height_ = texInfos[0].height;
#ifdef _DEBUG
	assert(width_ <= 4096 && height_ <= 4096);
#endif

	for (uint8_t i = 0; i < numRT; ++i) {
		assert(texInfos[i].width == width_ && texInfos[i].height == height_);
		uint64_t flags = UnpackTexFlags(static_cast<uint16_t>(texInfos[i].storageSize));
		assert(flags == BGFX_TEXTURE_RT);
		rTexs_.emplace_back(texInfos[i], flags);
	}

	Reset();
}


FrameBuffer::~FrameBuffer()
{
	Lost();
}


void FrameBuffer::Reset()
{
	uint8_t const numRT = static_cast<uint8_t>(rTexs_.size());
	if (numRT == 0)
		return;
	assert(!isValid(handle_));

	bgfx::TextureHandle rts[8];

	for (uint8_t i = 0; i < numRT; ++i) {
		assert(rTexs_[i].Flags() == BGFX_TEXTURE_RT);
		rts[i] = bgfx::createTexture2D(width_, height_, false, 1, rTexs_[i].Format(), rTexs_[i].Flags());
		rTexs_[i].Handle(rts[i], true);
	}
	handle_ = bgfx::createFrameBuffer(numRT, rts, true);
}


void FrameBuffer::Lost()
{
	if (isValid(handle_)) {
		bgfx::destroy(handle_);
		handle_ = BGFX_INVALID_HANDLE;
	}
}


void FrameBuffer::Setup(Camera const *pCam, bgfx::ViewMode::Enum mode,
	uint16_t clearFlags , uint32_t rgba, float d, uint8_t s) const
{
	uint8_t viewID;
	if (s_currFB == this && mode == bgfx::ViewMode::Sequential)
		viewID = s_viewChannelID;
	else {
		viewID = ++s_viewChannelID;
		if (0 == viewID)
			bgfx::frame();
	}

	bgfx::setViewRect(viewID, 0, 0, width_, height_);
	bgfx::setViewFrameBuffer(viewID, handle_);
	bgfx::setViewMode(viewID, mode);
	if (pCam)
		bgfx::setViewTransform(viewID, pCam->mtxView.v, pCam->mtxProj.v);
	else
		bgfx::setViewTransform(viewID, nullptr, s_mtxOrtho.v);
	
	bgfx::setViewClear(viewID, clearFlags, rgba, d, s);

	s_currFB = this;
}


inline uint64_t MakeFrameBufferKey(uint16_t w, uint16_t h, bgfx::TextureFormat::Enum fmt, uint64_t flags, uint8_t mipCnt)
{
	uint64_t key = (w & 0xffff) | ((h & 0xffff) << 16);	// may 0xfff (4k) be enough?
	key |= static_cast<uint64_t>(fmt) << 32;
	key |= static_cast<uint64_t>(mipCnt) << 40;
	key |= static_cast<uint64_t>(PackTexFlags(flags)) << 44;
	return key;
}


uint64_t FrameBuffer::HashCode() const
{
	if (rTexs_.size() > 1)
		return 0;
	Texture const & rt = rTexs_[0];
	return MakeFrameBufferKey(rt.Width(), rt.Height(), rt.Format(), rt.Flags(), rt.NumMips());
}


FrameBuffer const * FrameBuffer::CheckOut(uint16_t w, uint16_t h, bgfx::TextureFormat::Enum fmt, uint64_t flags, uint8_t mipCnt)
{
	assert(w * h > 0 && fmt != bgfx::TextureFormat::Enum::Unknown);
	uint64_t key = MakeFrameBufferKey(w, h, fmt, flags, mipCnt);

	FrameBuffer const * ret = nullptr;
	for (auto & pFB : s_rts) {
		if (pFB->HashCode() == key) {
			ret = pFB;
			break;
		}
	}
	if (ret != nullptr)
		s_rts.erase(ret);
	else
		ret = new FrameBuffer(w, h, fmt, flags, mipCnt);

	return ret;
}


void FrameBuffer::CheckIn(FrameBuffer const * &pFB)
{
	assert(pFB->rTexs_.size() == 1);
	s_rts.insert(pFB);
	pFB = nullptr;
}


void FrameBuffer::Update()
{
	s_viewChannelID = 0;
	// Advance to next frame. Rendering thread will be kicked to
	// process submitted rendering primitives.
	bgfx::frame();
}

}
