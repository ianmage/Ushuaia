#pragma once

#include <array>
#include <vector>
#include "vtxDecl.h"
#include "shader.h"


namespace Ushuaia
{

void DrawScreenQuad(bgfx::ViewId viewId, Shader const *pShader
	, uint64_t state, float x, float y, float w, float h);

typedef std::array<Vector3, 8>	FrustumCorners;

void WorldSpaceFrustumCorners(FrustumCorners & corners
	, float near, float far, float projWidth, float projHeight
	, Matrix4x4 const & mtxInvView);

void SplitFrustum(std::vector<float> & splits, uint8_t numSplits
	, float near, float far, float splitWeight = 0.75f);

void CalcMeshNormal(Vector3 * normals, uint16_t numNorm
	, Vector3 const * vertices, uint16_t numVtx
	, uint16_t const * indices, uint32_t numIdx);

void CreateCuboid(std::vector<PosNormVertex> & vtxOut, std::vector<uint16_t> & idxOut
	, Vector3 const & length);

void CreateSphere(std::vector<PosNormVertex> & vtxOut, std::vector<uint16_t> & idxOut
	, uint8_t lod, float scale);

uint16_t FloatToUint16(float f);

}
