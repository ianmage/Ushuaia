#pragma once

#include <array>
#include "PostProcess.h"


namespace Ushuaia
{

class GaussianBlur : public PostProcessor
{
public :
	static GaussianBlur& Instance() { return s_instance; }

	void Render(Texture const *pSrcTex, FrameBuffer const *pOutFB) override;

private :
	GaussianBlur();
	bool Init() override;
	void Fini() override;
	void CalcSampleOffsets(float deviation);

	static uint8_t const SAMPLE_RADIUS = 8;

	uint8_t kernelRadius_;
	float multiplier_;
	std::array<float, SAMPLE_RADIUS> tcOffsets_;
	std::array<float, SAMPLE_RADIUS> colorWeights_;
	Shader *pTechX_, *pTechY_;

	static GaussianBlur s_instance;
};

}
