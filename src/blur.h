#pragma once

#include "postProcess.h"
#include <array>


namespace Ushuaia
{

class GaussianBlur : public PostProcessor
{
public :
	void Render(Texture const *pSrcTex, FrameBuffer const *pOutFB) override;
	static GaussianBlur& Instance() { return s_instance; }

private :
	GaussianBlur();
	bool Init();
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
