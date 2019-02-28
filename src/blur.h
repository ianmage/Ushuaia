#pragma once

#include "shader.h"
#include "framebuffer.h"
#include <array>


namespace Ushuaia
{

class GaussianBlur
{
public :
	static void Render(Texture const & srcTex, FrameBuffer const *pOutFB);

private :
	static bool Init();
	static void CalcSampleOffsets(float deviation);

	static uint8_t const SAMPLE_RADIUS = 8;

	static uint8_t kernelRadius_;
	static float multiplier_;
	static std::array<float, SAMPLE_RADIUS> tcOffsets_;
	static std::array<float, SAMPLE_RADIUS> colorWeights_;
	static Shader *pTechX_, *pTechY_;
};

}
