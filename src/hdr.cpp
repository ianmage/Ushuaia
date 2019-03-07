#include "hdr.h"
#include "viewState.h"
#include "camera.h"
#include "bx/math.h"
#include "renderUtil.h"
#include "postProcess.h"
#include "../../cpp_common/commUtil.h"


namespace Ushuaia
{

static bgfx::UniformHandle uhViewVec;
static bgfx::UniformHandle uhRtSize;
static bgfx::UniformHandle uhOffsets;


static inline void UpdateScreenParams()
{
	float w = FrameBuffer::CurrFB()->Width();
	float h = FrameBuffer::CurrFB()->Height();
	Vector4 vec4Param {
		w, h,
		ViewState::texelOffset / w,
		ViewState::texelOffset / h
	};
	bgfx::setUniform(uhRtSize, &vec4Param);
}

static inline void UpdateViewParams()
{
	Vector3 vVec = Camera::pCurrent->ViewExpansionVector();
	Vector4 vec4Param { vVec.x, vVec.y, vVec.z, 0.f };
	bgfx::setUniform(uhViewVec, &vec4Param);
}


static void ZoomOutQuater(uint16_t* pW, uint16_t* pH)
{
	uint16_t w = *pW;
	w /= 4;
	if (w == 0)
		w = 1;
	uint16_t h = *pH;
	h /= 4;
	if (h == 0)
		h = 1;
	*pW = w;
	*pH = h;
}


struct ImageStatProcessor
{
	static FrameBuffer const * Output(Texture const *pTex);
	static FrameBuffer const * SumLumAverage(Texture const *pTex);
	static FrameBuffer const * SumLum(Texture const *pTex);

	static bool Init();
	static void Fini();

	static Shader *pLumTech_;
	static Shader *pLumAvgTech_;
};

decltype(ImageStatProcessor::pLumTech_) ImageStatProcessor::pLumTech_;
decltype(ImageStatProcessor::pLumAvgTech_) ImageStatProcessor::pLumAvgTech_;


bool ImageStatProcessor::Init()
{
	pLumTech_ = Shader::Load("screen/vs_screen_quad", "screen/HDR/fs_sum_lum");
	pLumAvgTech_ = Shader::Load("screen/vs_screen_quad", "screen/HDR/fs_sum_lum_avg");

	return true;
}


void ImageStatProcessor::Fini()
{
	SafeDelete(pLumTech_);
	SafeDelete(pLumAvgTech_);
}


FrameBuffer const * ImageStatProcessor::SumLumAverage(Texture const *pTex)
{
	FrameBuffer const *pFB = nullptr, *pOldFB = nullptr;
	bgfx::TextureHandle hTex = pTex->Handle();
	uint16_t w = pTex->Width(), h = pTex->Height();
	while (w > 1 || h > 1) {
		float u = 1.0f / w, v = 1.0f / h;
		Vector2 offsets[4] = {
			{-u, -v}, {+u, -v},
			{+u, +v}, {-u, +v}
		};
		ZoomOutQuater(&w, &h);
		pFB = new FrameBuffer(w, h, bgfx::TextureFormat::R16F);
		pFB->Setup(nullptr, bgfx::ViewMode::Sequential, false);
		bgfx::setUniform(uhOffsets, offsets, 2);
		bgfx::setTexture(0, SamplerMgr::Get("s_tex0"), hTex);
		bgfx::setState(BGFX_STATE_WRITE_R);
		PostProcess::DrawFullScreen(pLumAvgTech_);
		delete pOldFB;
		pOldFB = pFB;
		hTex = pFB->pTex(0)->Handle();
	}
	return pFB;
}


FrameBuffer const * ImageStatProcessor::SumLum(Texture const *pTex)
{
	uint16_t w = pTex->Width(), h = pTex->Height();
	float u = 1.0f / w, v = 1.0f / h;
	Vector2 offsets[4] = {
		{-u, -v}, {+u, -v},
		{+u, +v}, {-u, +v}
	};
	ZoomOutQuater(&w, &h);
	FrameBuffer const * pLum0FB = FrameBuffer::CheckOut(w, h, bgfx::TextureFormat::R16F);
	pLum0FB->Setup(nullptr, bgfx::ViewMode::Sequential, false);
	bgfx::setUniform(uhOffsets, offsets, 2);
	bgfx::setTexture(0, SamplerMgr::Get("s_tex0"), pTex->Handle());
	bgfx::setState(BGFX_STATE_WRITE_R);
	PostProcess::DrawFullScreen(pLumTech_);

	return pLum0FB;
}


FrameBuffer const * ImageStatProcessor::Output(Texture const *pTex)
{
	FrameBuffer const * pLum0FB = ImageStatProcessor::SumLum(pTex);
	FrameBuffer const * pFB = SumLumAverage(pLum0FB->pTex(0));

	FrameBuffer::CheckIn(pLum0FB);

	return pFB;
}


struct LensEffect
{
	static FrameBuffer const * Output(Texture const *pTex);
	static FrameBuffer const * BrightPass(Texture const *pTex);

	static bool Init();
	static void Fini();

	static Shader *pBrightTech_;
};

decltype(LensEffect::pBrightTech_) LensEffect::pBrightTech_;


decltype(HDR::s_instance) HDR::s_instance;


bool HDR::Init()
{
	uhViewVec = bgfx::createUniform("PV_viewVec", bgfx::UniformType::Vec4);
	uhRtSize = bgfx::createUniform("PV_rtSize", bgfx::UniformType::Vec4);
	uhOffsets = bgfx::createUniform("uTexCoordOffset", bgfx::UniformType::Vec4, 2);

	ImageStatProcessor::Init();

	return true;
}


void HDR::Fini()
{
	SafeDestroy(uhViewVec);
	SafeDestroy(uhRtSize);

	ImageStatProcessor::Fini();
}


void HDR::Render(Texture const *pInTex, FrameBuffer const * pOutFB)
{
	if (pTech_ == nullptr)
		Init();

	FrameBuffer const * pLumFB = ImageStatProcessor::Output(pInTex);


	delete pLumFB;
}

}
