#include "frameBuffer.h"
#include "viewState.h"
#include <cassert>


namespace Ushuaia
{

uint16_t FrameBuffer::s_viewCnt = 0;
FrameBuffer const * FrameBuffer::s_currFB = nullptr;
FrameBuffer const * FrameBuffer::s_backBuf = nullptr;
static Matrix4x4 s_mtxOrtho;


bool FrameBuffer::Init()
{
	bgfx::Caps const * caps = bgfx::getCaps();
	bx::mtxOrtho(s_mtxOrtho.v, 0.f, 1.f, 1.f, 0.f, 0.f, 100.f, 0.f, caps->homogeneousDepth);
	assert(!s_backBuf);
	s_backBuf = new FrameBuffer(g_viewState.width, g_viewState.height, bgfx::TextureFormat::Unknown);
	s_currFB = s_backBuf;

	return true;
}


void FrameBuffer::Fini()
{
	delete s_backBuf;
	s_backBuf = nullptr;
}


FrameBuffer::FrameBuffer(uint16_t w, uint16_t h, bgfx::TextureFormat::Enum fmt)
: handle_(BGFX_INVALID_HANDLE)
, width_(w), height_(h)
{
	viewID_ = s_viewCnt++;

	bgfx::TextureInfo texInfo;
	texInfo.width = w;
	texInfo.height = h;
	texInfo.depth = 1;
	texInfo.numLayers = 1;
	texInfo.numMips = 1;
	texInfo.format = fmt;
	texInfo.cubeMap = false;

	rTexs_.emplace_back(texInfo);
}


FrameBuffer::FrameBuffer(uint16_t w, uint16_t h, uint8_t num, bgfx::TextureFormat::Enum const * fmts)
: handle_(BGFX_INVALID_HANDLE)
, width_(w), height_(h)
{
	assert(num > 0 && num <= 8);

	viewID_ = s_viewCnt++;

	bgfx::TextureInfo texInfo;
	for (uint8_t i = 0; i < num; ++i) {
		texInfo.width = w;
		texInfo.height = h;
		texInfo.depth = 1;
		texInfo.numLayers = 1;
		texInfo.numMips = 1;
		texInfo.format = fmts[i];
		texInfo.cubeMap = false;

		rTexs_.emplace_back(texInfo);
	}
}


FrameBuffer::~FrameBuffer()
{
	Lost();
}


void FrameBuffer::Reset()
{
	size_t const numRT = rTexs_.size();
	if (numRT == 0)
		return;
	assert(!isValid(handle_));

	uint64_t const samplerFlags = 0
		| BGFX_TEXTURE_RT
		| BGFX_SAMPLER_MIN_POINT
		| BGFX_SAMPLER_MAG_POINT
		| BGFX_SAMPLER_MIP_POINT
		| BGFX_SAMPLER_U_CLAMP
		| BGFX_SAMPLER_V_CLAMP
		;

	bgfx::TextureHandle rts[8];

	for (uint8_t i = 0; i < numRT; ++i) {
		rts[i] = bgfx::createTexture2D(width_, height_, false, 1, rTexs_[i].Format(), samplerFlags);
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


void FrameBuffer::Setup(Camera const *pCam, bgfx::ViewMode::Enum mode, bool doClear) const
{
	bgfx::setViewRect(viewID_, 0, 0, width_, height_);
	bgfx::setViewFrameBuffer(viewID_, handle_);
	bgfx::setViewMode(viewID_, mode);
	if (pCam)
		bgfx::setViewTransform(viewID_, pCam->mtxView.v, pCam->mtxProj.v);
	else
		bgfx::setViewTransform(viewID_, nullptr, s_mtxOrtho.v);
	
	if (doClear)
		bgfx::setViewClear(viewID_,
			BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0, 1.f, 0);

	s_currFB = this;
}

}
