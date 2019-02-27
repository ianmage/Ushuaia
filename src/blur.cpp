#include "blur.h"
#include "renderUtil.h"
#include <array>
#include "postProcess.h"

#pragma optimize("", off)


namespace Ushuaia
{

Shader* GaussianBlur::pTechX = nullptr;
Shader* GaussianBlur::pTechY = nullptr;

static bgfx::UniformHandle uhTexSize;
static bgfx::UniformHandle uhColorWeights;
static bgfx::UniformHandle uhTcOffsets;


bool GaussianBlur::Init()
{
	uhTexSize = bgfx::createUniform("texSize", bgfx::UniformType::Vec4);
	uhColorWeights = bgfx::createUniform("colorWeights", bgfx::UniformType::Vec4, 2);
	uhTcOffsets = bgfx::createUniform("tcOffsets", bgfx::UniformType::Vec4, 2);

	pTechX = Shader::Load("screen/vs_screen_quad", "screen/SeparableBlur/fs_gaussian_blur_x");
	pTechY = Shader::Load("screen/vs_screen_quad", "screen/SeparableBlur/fs_gaussian_blur_y");

	return true;
}


static float GaussianDistribution(float x, float y, float rho)
{
	float g = 1.0f / sqrt(2.0f * MATH_PI * rho * rho);
	g *= exp(-(x * x + y * y) / (2 * rho * rho));
	return g;
}


static void CalcSampleOffsets(uint16_t texSize, uint8_t kernelRadius, float deviation)
{
	std::array<float, 8> colorWeights = { 0 };
	std::array<float, 8> tcOffsets = { 0 };

	std::vector<float> tmpWeights(kernelRadius * 2, 0);
	std::vector<float> tmpOffsets(kernelRadius * 2, 0);

	float const tu = 1.0f / texSize;

	float sumWeight = 0;
	for (int i = 0; i < 2 * kernelRadius; ++i) {
		float xCoord = static_cast<float>(i - kernelRadius);
		float weight = GaussianDistribution(xCoord, 0, kernelRadius / deviation);
		tmpWeights[i] = weight;
		sumWeight += weight;
	}
	for (int i = 0; i < 2 * kernelRadius; ++i)
		tmpWeights[i] /= sumWeight;

	// fill the offsets
	for (int i = 0; i < kernelRadius; ++i) {
		tmpOffsets[i] = static_cast<float>(i - kernelRadius);
		tmpOffsets[i + kernelRadius] = static_cast<float>(i);
	}

	// bilinear filtering taps, ordering left to right
	for (int i = 0; i < kernelRadius; ++i) {
		float const scale = tmpWeights[i * 2] + tmpWeights[i * 2 + 1];
		float const frac = tmpWeights[i * 2] / scale;

		tcOffsets[i] = (tmpOffsets[i * 2] + (1 - frac)) * tu;
		colorWeights[i] = scale;
	}
}


void GaussianBlur::Render(Texture const & srcTex, FrameBuffer const *pOutFB)
{
	if (pTechX == nullptr)
		Init();

	bgfx::setMarker("Gaussian Blur X");
	Texture const & rTex = pOutFB->Tex(0);

	FrameBuffer const * pTmpFB = FrameBuffer::CheckOut(rTex.Width(), rTex.Height(), rTex.Format());

	pTmpFB->Setup(nullptr, bgfx::ViewMode::Sequential, false);
	bgfx::setTexture(0, SamplerMgr::Get("s_tex0"), srcTex.Handle());
	bgfx::setState(BGFX_STATE_WRITE_RGB);
	PostProcess::DrawFullScreen(pTechX);

	pOutFB->Setup(nullptr, bgfx::ViewMode::Sequential, false);
	bgfx::setTexture(0, SamplerMgr::Get("s_tex0"), pTmpFB->Tex(0).Handle());
	bgfx::setState(BGFX_STATE_WRITE_RGB);
	PostProcess::DrawFullScreen(pTechY);
}

}
