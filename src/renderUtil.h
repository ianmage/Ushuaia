#pragma once

#include <array>
#include <vector>
#include "shader.h"
#include "camera.h"
#include "texture.h"
#include "postProcess.h"


namespace Ushuaia
{

inline void SafeDestroy(bgfx::UniformHandle uh)
{
	if (isValid(uh)) {
		bgfx::destroy(uh);
		uh = BGFX_INVALID_HANDLE;
	}
}

inline void SetTexture(uint8_t stage, std::string const & name, Texture const *pTex, uint32_t samplerFlag) {
	bgfx::TextureHandle hTex = BGFX_INVALID_HANDLE;
	if (pTex)
		hTex = pTex->Handle();
	bgfx::setTexture(stage, SamplerMgr::Get(name), hTex, samplerFlag);
}


void DrawScreenQuad(Shader const *pShader, uint64_t state
	, float x, float y, float w, float h);

typedef std::array<Vector3, 8>	FrustumCorners;

void WorldSpaceFrustumCorners(FrustumCorners & corners
	, float near, float far, float projWidth, float projHeight
	, Matrix4x4 const & mtxInvView);

void SplitFrustum(std::vector<float> & splits, uint8_t numSplits
	, float near, float far, float splitWeight = 0.75f);

void CalcMeshNormal(Vector3 * normals, uint16_t numNorm
	, Vector3 const * vertices, uint16_t numVtx
	, uint16_t const * indices, uint32_t numIdx);

uint16_t FloatToUint16(float f);


void Copy(Texture const *pSrcTex, FrameBuffer const *pOutFB, uint64_t writeState, bool bilinear);

void DownQuater(Texture const *pTex, FrameBuffer const *pOutFB, uint64_t writeState);

void ClearRenderUtilRes();

}
