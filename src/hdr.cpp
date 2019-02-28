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

static Shader *pLumTech = nullptr;
static Shader *pLumAvgTech = nullptr;


void HDR::Init()
{
	uhViewVec = bgfx::createUniform("PV_viewVec", bgfx::UniformType::Vec4);
	uhRtSize = bgfx::createUniform("PV_rtSize", bgfx::UniformType::Vec4);
	uhOffsets = bgfx::createUniform("uTexCoordOffset", bgfx::UniformType::Vec4, 2);

	pLumTech = Shader::Load("screen/vs_screen_quad", "screen/HDR/fs_sum_lum");
	pLumAvgTech = Shader::Load("screen/vs_screen_quad", "screen/HDR/fs_sum_lum_avg");

	Reset();
}


void HDR::Fini()
{
	Lost();

	bgfx::destroy(uhViewVec);
	bgfx::destroy(uhRtSize);
}


void HDR::Lost()
{
}


void HDR::Reset()
{
}


void HDR::Update()
{
}


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


static void ZoomOut(uint16_t* pW, uint16_t* pH)
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


static FrameBuffer const * SumLumAverage(Texture const & tex)
{
	FrameBuffer const *pFB = nullptr, *pOldFB = nullptr;
	bgfx::TextureHandle hTex = tex.Handle();
	uint16_t w = tex.Width(), h = tex.Height();
	while (w > 1 || h > 1) {
		float u = 1.0f / w, v = 1.0f / h;
		Vector2 offsets[4] = {
			{-u, -v}, {+u, -v},
			{+u, +v}, {-u, +v}
		};
		ZoomOut(&w, &h);
		pFB = new FrameBuffer(w, h, bgfx::TextureFormat::R16F);
		pFB->Setup(nullptr, bgfx::ViewMode::Sequential, false);
		bgfx::setUniform(uhOffsets, offsets, 2);
		bgfx::setTexture(0, SamplerMgr::Get("s_tex0"), hTex);
		bgfx::setState(BGFX_STATE_WRITE_R);
		PostProcess::DrawFullScreen(pLumAvgTech);
		delete pOldFB;
		pOldFB = pFB;
		hTex = pFB->Tex(0).Handle();
	}
	return pFB;
}


static FrameBuffer const * SumLum(Texture const & tex)
{
	uint16_t w = tex.Width(), h = tex.Height();
	float u = 1.0f / w, v = 1.0f / h;
	Vector2 offsets[4] = {
		{-u, -v}, {+u, -v},
		{+u, +v}, {-u, +v}
	};
	ZoomOut(&w, &h);
	FrameBuffer const * pLum0FB = new FrameBuffer(w, h, bgfx::TextureFormat::R16F);
	pLum0FB->Setup(nullptr, bgfx::ViewMode::Sequential, false);
	bgfx::setUniform(uhOffsets, offsets, 2);
	bgfx::setTexture(0, SamplerMgr::Get("s_tex0"), tex.Handle());
	bgfx::setState(BGFX_STATE_WRITE_R);
	PostProcess::DrawFullScreen(pLumTech);

	FrameBuffer const * pFB = SumLumAverage(pLum0FB->Tex(0));

	delete pLum0FB;

	return pFB;
}


void HDR::Render(Texture const & inTex, FrameBuffer const * pOutFB)
{
	FrameBuffer const * pLumFB = SumLum(inTex);


	delete pLumFB;
}

}
