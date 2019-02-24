#include "renderUtil.h"
#include "viewState.h"
#include "bx/math.h"
#include "vtxDecl.h"
#include "../examples/common/bgfx_utils.h"
#include <cmath>
#include "math.h"
#include <map>
#include "frameBuffer.h"
#include <array>

#pragma optimize("", off)


namespace Ushuaia
{

static float GaussianDistribution(float x, float y, float rho)
{
	float g = 1.0f / sqrt(2.0f * MATH_PI * rho * rho);
	g *= exp(-(x * x + y * y) / (2 * rho * rho));
	return g;
}


static void CalcSampleOffsets(uint16_t texSize, float kernelRadius, float deviation)
{
	std::array<float, 6> colorWeights;
	std::array<float, 6> tcOffsets;

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
	for (int i = kernelRadius; i < 8; ++i) {
		tcOffsets[i] = 0.0f;
		colorWeights[i] = 0.0f;
	}
}


static void MakeScreenQuad(void * pVerts,
	float x, float y, float w, float h)
{
	x = x * 2.f - 1.f;
	y = 1.f - y * 2.f;
	w *= 2.f;
	h *= 2.f;
	float const zz = 0.f;

	TRect<float> xyRect { { x, y-h }, { x+w, y } };

	PosVertex *pVtx = reinterpret_cast<PosVertex*>(pVerts);

	pVtx[0].pos.Set(xyRect.rMin.x, xyRect.rMax.y, zz);
	pVtx[1].pos.Set(xyRect.rMax.x, xyRect.rMax.y, zz);
	pVtx[2].pos.Set(xyRect.rMin.x, xyRect.rMin.y, zz);
	pVtx[3].pos.Set(xyRect.rMax.x, xyRect.rMin.y, zz);
}


void GaussianBlur(Shader const *pShader
	, uint64_t state, float x, float y, float w, float h)
{
	assert(4 == bgfx::getAvailTransientVertexBuffer(4, PosVertex::s_decl));
	bgfx::TransientVertexBuffer vb;
	bgfx::allocTransientVertexBuffer(&vb, 4, PosVertex::s_decl);

	MakeScreenQuad(vb.data, x, y, w, h);

	bgfx::setVertexBuffer(0, &vb);

	state |= BGFX_STATE_PT_TRISTRIP;
	bgfx::setState(state);

	bgfx::submit(FrameBuffer::CurrFB()->ViewID(), pShader->Tech());
}

}
