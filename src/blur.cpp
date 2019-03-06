#include "blur.h"
#include "renderUtil.h"
#include "postProcess.h"

#pragma optimize("", off)


namespace Ushuaia
{

uint8_t GaussianBlur::kernelRadius_ = 8;
float GaussianBlur::multiplier_ = 1.0f;
decltype(GaussianBlur::tcOffsets_) GaussianBlur::tcOffsets_;
std::array<float, GaussianBlur::SAMPLE_RADIUS> GaussianBlur::colorWeights_;
Shader* GaussianBlur::pTechX_ = nullptr;
Shader* GaussianBlur::pTechY_ = nullptr;

static bgfx::UniformHandle uhTexSize;
static bgfx::UniformHandle uhColorWeights;
static bgfx::UniformHandle uhTcOffsets;


bool GaussianBlur::Init()
{
	uhTexSize = bgfx::createUniform("texSize", bgfx::UniformType::Vec4);
	uhColorWeights = bgfx::createUniform("colorWeights", bgfx::UniformType::Vec4, 2);
	uhTcOffsets = bgfx::createUniform("tcOffsets", bgfx::UniformType::Vec4, 2);

	pTechX_ = Shader::Load("screen/vs_screen_quad", "screen/SeparableBlur/fs_gaussian_blur_x");
	pTechY_ = Shader::Load("screen/vs_screen_quad", "screen/SeparableBlur/fs_gaussian_blur_y");

	CalcSampleOffsets(3.0f);

	return true;
}


static float GaussianDistribution(float x, float y, float rho)
{
	float const pi = static_cast<float>(MATH_PI);
	float g = 1.0f / sqrtf(2.0f * pi * rho * rho);
	g *= exp(-(x * x + y * y) / (2 * rho * rho));
	return g;
}


void GaussianBlur::CalcSampleOffsets(float deviation)
{
	std::vector<float> tmpWeights(kernelRadius_ * 2, 0);
	std::vector<float> tmpOffsets(kernelRadius_ * 2, 0);

	float sumWeight = 0;
	for (int i = 0; i < 2 * kernelRadius_; ++i) {
		float xCoord = static_cast<float>(i - kernelRadius_);
		float weight = GaussianDistribution(xCoord, 0, kernelRadius_ / deviation);
		tmpWeights[i] = weight;
		sumWeight += weight;
	}
	for (int i = 0; i < 2 * kernelRadius_; ++i)
		tmpWeights[i] /= sumWeight;

	// fill the offsets
	for (int i = 0; i < kernelRadius_; ++i) {
		tmpOffsets[i] = static_cast<float>(i - kernelRadius_);
		tmpOffsets[i + kernelRadius_] = static_cast<float>(i);
	}

	// bilinear filtering taps, ordering left to right
	for (int i = 0; i < kernelRadius_; ++i) {
		float const scale = tmpWeights[i * 2] + tmpWeights[i * 2 + 1];
		float const frac = tmpWeights[i * 2] / scale;

		tcOffsets_[i] = tmpOffsets[i * 2] + (1 - frac);
		colorWeights_[i] = multiplier_ * scale;
	}
	for (int i = kernelRadius_; i < SAMPLE_RADIUS; ++i) {
		colorWeights_[i] = 0.0f;
		tcOffsets_[i] = 0.0f;
	}
}


void GaussianBlur::Render(Texture const & srcTex, FrameBuffer const *pOutFB)
{
	if (pTechX_ == nullptr)
		Init();
	float srcW = static_cast<float>(srcTex.Width());
	float srcH = static_cast<float>(srcTex.Height());
	Vector4 texSize{ srcW, srcH, 1.0f / srcW, 1.0f / srcH };

	bgfx::setMarker("Gaussian Blur X");
	Texture const & rTex = pOutFB->Tex(0);

	FrameBuffer const * pTmpFB = FrameBuffer::CheckOut(rTex.Width(), rTex.Height(), rTex.Format());

	pTmpFB->Setup(nullptr, bgfx::ViewMode::Sequential, false);
	bgfx::setUniform(uhTexSize, texSize.v);
	bgfx::setUniform(uhTcOffsets, tcOffsets_.data(), 2);
	bgfx::setUniform(uhColorWeights, colorWeights_.data(), 2);
	bgfx::setTexture(0, SamplerMgr::Get("s_tex0"), srcTex.Handle());
	bgfx::setState(BGFX_STATE_WRITE_RGB);
	PostProcess::DrawFullScreen(pTechX_);

	bgfx::setMarker("Gaussian Blur Y");

	pOutFB->Setup(nullptr, bgfx::ViewMode::Sequential, false);
	bgfx::setUniform(uhTexSize, texSize.v);
	bgfx::setUniform(uhTcOffsets, tcOffsets_.data(), 2);
	bgfx::setUniform(uhColorWeights, colorWeights_.data(), 2);
	bgfx::setTexture(0, SamplerMgr::Get("s_tex0"), pTmpFB->Tex(0).Handle());
	bgfx::setState(BGFX_STATE_WRITE_RGB);
	PostProcess::DrawFullScreen(pTechY_);

	FrameBuffer::CheckIn(pTmpFB);
}

}