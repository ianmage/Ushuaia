#include "hdr.h"
#include "viewState.h"
#include "camera.h"
#include "bx/math.h"
#include "renderUtil.h"
#include "postProcess.h"
#include "frameBuffer.h"
#include "../../cpp_common/commUtil.h"


namespace Ushuaia
{

static bgfx::UniformHandle uhViewVec;
static bgfx::UniformHandle uhRtSize;
static bgfx::UniformHandle uhOffsets;
static bgfx::UniformHandle s_Sampler[3];

static Shader *pLumTech = nullptr;
static Shader *pLumAvgTech = nullptr;


void HDR::Init()
{
	std::string const smpl = "s_tex";
	for (uint8_t i = 0; i < ArrayCount(s_Sampler); ++i) {
		s_Sampler[i] = bgfx::createUniform((smpl+std::to_string(i)).c_str()
			, bgfx::UniformType::Sampler);
	}
	uhViewVec = bgfx::createUniform("PV_viewVec", bgfx::UniformType::Vec4);
	uhRtSize = bgfx::createUniform("PV_rtSize", bgfx::UniformType::Vec4);
	uhOffsets = bgfx::createUniform("uvOffsets", bgfx::UniformType::Vec4, 2);

	pLumTech = Shader::Load("screen/vs_screen_quad", "screen/hdr/fs_lum");
	pLumAvgTech = Shader::Load("screen/vs_screen_quad", "screen/hdr/fs_lumavg");

	Reset();
}


void HDR::Fini()
{
	Lost();

	bgfx::destroy(uhViewVec);
	bgfx::destroy(uhRtSize);
	for (uint8_t i = 0; i < ArrayCount(s_Sampler); ++i) {
		if (isValid(s_Sampler[i]))
			bgfx::destroy(s_Sampler[i]);
	}
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


static FrameBuffer* CalcLumAverage(Texture const & tex)
{
	FrameBuffer *pFB = nullptr;
	uint16_t w = tex.Width(), h = tex.Height();
	while (w > 1 || h > 1) {
		float u = 1.0f / w, v = 1.0f / h;
		Vector2 offsets[4] = {
			{-u, -v}, {+u, -v},
			{+u, +v}, {-u, +v}
		};
		ZoomOut(&w, &h);
		pFB = new FrameBuffer(w, h, bgfx::TextureFormat::BGRA8);
		pFB->Setup(nullptr, bgfx::ViewMode::Sequential, false);
		bgfx::setUniform(uhOffsets, offsets, 2);
		bgfx::setTexture(0, s_Sampler[0], tex.Handle());
		bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);
		PostProcess::DrawFullScreen(pLumAvgTech);
	}
}


static FrameBuffer* CalcLum(Texture const & tex)
{
	FrameBuffer *pFB = nullptr;
	uint16_t w = tex.Width(), h = tex.Height();
	float u = 1.0f / w, v = 1.0f / h;
	Vector2 offsets[4] = {
		{-u, -v}, {+u, -v},
		{+u, +v}, {-u, +v}
	};
	ZoomOut(&w, &h);
	pFB = new FrameBuffer(w, h, bgfx::TextureFormat::BGRA8);
	pFB->Setup(nullptr, bgfx::ViewMode::Sequential, false);
	bgfx::setUniform(uhOffsets, offsets, 2);
	bgfx::setTexture(0, s_Sampler[0], tex.Handle());
	bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);
	PostProcess::DrawFullScreen(pLumTech);

	pFB = CalcLumAverage(pFB->Tex(0));

	return pFB;
}


void HDR::Render(Texture const & inTex, FrameBuffer const * pOutFB)
{
	FrameBuffer* pLumFB = CalcLum(inTex);


	delete pLumFB;
}

}
