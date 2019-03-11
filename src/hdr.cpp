#include "hdr.h"
#include "viewState.h"
#include "camera.h"
#include "bx/math.h"
#include "renderUtil.h"
#include "postProcess.h"
#include "../../cpp_common/commUtil.h"
#include "blur.h"


namespace Ushuaia
{

static bgfx::UniformHandle hParam;
static bgfx::UniformHandle hTcOffsets;


static inline void UpdateScreenParams()
{
	float w = FrameBuffer::CurrFB()->Width();
	float h = FrameBuffer::CurrFB()->Height();
	Vector4 vec4Param {
		w, h,
		ViewState::texelOffset / w,
		ViewState::texelOffset / h
	};
	bgfx::setUniform(hParam, &vec4Param);
}


class ImageStatProcessor
{
public :
	static FrameBuffer const * SumLum(Texture const *pTex);

	static void Fini();

private :
	static Shader *pTech_;
};

decltype(ImageStatProcessor::pTech_) ImageStatProcessor::pTech_ = nullptr;


void ImageStatProcessor::Fini()
{
	SafeDelete(pTech_);
}


FrameBuffer const * ImageStatProcessor::SumLum(Texture const *pTex)
{
	if (pTech_ == nullptr)
		pTech_ = Shader::Load("screen/vs_down_quater", "screen/HDR/fs_sum_lum");

	uint16_t w = pTex->Width(), h = pTex->Height();
	float u = 1.0f / w, v = 1.0f / h;
	Vector2 offsets[4] = {
		{-u, -v}, {+u, -v},
		{+u, +v}, {-u, +v}
	};
	uint32_t samplerFlag = BGFX_SAMPLER_UVW_CLAMP | BGFX_SAMPLER_MIP_POINT;

	w = static_cast<uint16_t>(std::max(1, w >> 2));
	h = static_cast<uint16_t>(std::max(1, h >> 2));
	FrameBuffer const * pLumFB = FrameBuffer::CheckOut(w, h, bgfx::TextureFormat::R16F);
	pLumFB->Setup(nullptr, bgfx::ViewMode::Sequential, 0);
	bgfx::setUniform(hTcOffsets, offsets, 2);
	SetTexture(0, "src_tex", pTex, samplerFlag);
	bgfx::setState(BGFX_STATE_WRITE_R);
	PostProcess::DrawFullScreen(pTech_);

	std::array<FrameBuffer const *, 2> pDownFBs = { pLumFB, nullptr };
	int i = 0;
	while (w > 1 || h > 1) {
		w = static_cast<uint16_t>(std::max(1, w >> 2));
		h = static_cast<uint16_t>(std::max(1, h >> 2));

		pDownFBs[1 - i] = FrameBuffer::CheckOut(w, h, bgfx::TextureFormat::R16F);
		DownQuater(pDownFBs[i]->pTex(), pDownFBs[1-i], BGFX_STATE_WRITE_R);
		FrameBuffer::CheckIn(pDownFBs[i]);
		i = 1 - i;
	}

	return pDownFBs[i];
}


class LensEffect
{
public :
	static FrameBuffer const * GlowMerger(Texture const *pTex);

	static void Fini();

private :
	static void BrightPass(Texture const *pTex, FrameBuffer const *pFB);

	static Shader *pBrightTech_;
	static Shader *pGlowMergeTech_;
};

decltype(LensEffect::pBrightTech_) LensEffect::pBrightTech_ = nullptr;
decltype(LensEffect::pGlowMergeTech_) LensEffect::pGlowMergeTech_ = nullptr;


void LensEffect::Fini()
{
	SafeDelete(pBrightTech_);
	SafeDelete(pGlowMergeTech_);
}


void LensEffect::BrightPass(Texture const *pTex, FrameBuffer const *pFB)
{
	if (pBrightTech_ == nullptr)
		pBrightTech_ = Shader::Load("screen/vs_screen_quad", "screen/HDR/fs_bright");

	pFB->Setup(nullptr, bgfx::ViewMode::Sequential, 0);
	uint32_t const linearSampler = BGFX_SAMPLER_UVW_CLAMP | BGFX_SAMPLER_MIP_POINT;
	SetTexture(0, "src_tex", pTex, linearSampler);
	bgfx::setState(BGFX_STATE_WRITE_RGB);
	PostProcess::DrawFullScreen(pBrightTech_);
}


FrameBuffer const * LensEffect::GlowMerger(Texture const *pTex)
{
	if (pGlowMergeTech_ == nullptr)
		pGlowMergeTech_ = Shader::Load("screen/vs_screen_quad", "screen/HDR/fs_glow_merger");

	uint16_t w = pTex->Width(), h = pTex->Height();
	bgfx::TextureFormat::Enum fmt = pTex->Format();

	std::array<FrameBuffer const *, 2> downFBs = {
		FrameBuffer::CheckOut(w/2, h/2, fmt),
		FrameBuffer::CheckOut(w/8, h/8, fmt)
	};
	std::array<FrameBuffer const *, 2> glowFBs = {
		FrameBuffer::CheckOut(w/2, h/2, fmt),
		FrameBuffer::CheckOut(w/8, h/8, fmt)
	};

	uint64_t const writeState = BGFX_STATE_WRITE_RGB;

	BrightPass(pTex, downFBs[0]);
	DownQuater(downFBs[0]->pTex(), downFBs[1], writeState);

	GaussianBlur::Instance().Render(downFBs[0]->pTex(), glowFBs[0]);
	GaussianBlur::Instance().Render(downFBs[1]->pTex(), glowFBs[1]);

	for (auto downFB : downFBs)
		FrameBuffer::CheckIn(downFB);

	FrameBuffer const *pFB = FrameBuffer::CheckOut(w/2, h/2, fmt);
	pFB->Setup(nullptr, bgfx::ViewMode::Sequential, 0);
	uint32_t const linearSampler = BGFX_SAMPLER_UVW_CLAMP | BGFX_SAMPLER_MIP_POINT;
	SetTexture(0, "glow_tex_0", glowFBs[0]->pTex(), linearSampler);
	SetTexture(1, "glow_tex_1", glowFBs[1]->pTex(), linearSampler);
	bgfx::setState(BGFX_STATE_WRITE_RGB);
	PostProcess::DrawFullScreen(pGlowMergeTech_);

	for (auto glowFB : glowFBs)
		FrameBuffer::CheckIn(glowFB);

	return pFB;
}


decltype(HDR::s_instance) HDR::s_instance;


bool HDR::Init()
{
	hTcOffsets = bgfx::createUniform("uTexCoordOffset", bgfx::UniformType::Vec4, 2);
	hParam = bgfx::createUniform("uParam", bgfx::UniformType::Vec4);

	pTech_ = Shader::Load("screen/HDR/vs_tone_mapping", "screen/HDR/fs_tone_mapping");

	return true;
}


void HDR::Fini()
{
	SafeDestroy(hTcOffsets);
	SafeDestroy(hParam);

	ImageStatProcessor::Fini();
	LensEffect::Fini();
}


void HDR::Render(Texture const *pTex, FrameBuffer const *pFB)
{
	if (pTech_ == nullptr)
		Init();

	FrameBuffer const *pLumFB = ImageStatProcessor::SumLum(pTex);
	FrameBuffer const *pGlowFB = LensEffect::GlowMerger(pTex);

	pFB->Setup(nullptr, bgfx::ViewMode::Sequential, 0);
	uint32_t const linearSampler = BGFX_SAMPLER_UVW_CLAMP | BGFX_SAMPLER_MIP_POINT;
	uint32_t const pointSampler = linearSampler | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT;
	SetTexture(0, "lum_tex", pLumFB->pTex(), pointSampler);
	SetTexture(0, "src_tex", pTex, linearSampler);
	SetTexture(1, "bloom_tex", pGlowFB->pTex(), linearSampler);
	bgfx::setState(BGFX_STATE_WRITE_RGB);
	PostProcess::DrawFullScreen(pTech_);

	FrameBuffer::CheckIn(pLumFB);
	FrameBuffer::CheckIn(pGlowFB);
}

}
