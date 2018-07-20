#pragma once

#include <array>
#include <vector>
#include "vtxDecl.h"


namespace Ushuaia
{

void ScreenSpaceQuad(float texW, float texH
	, bool origBottmLeft = true, float w = 1.f, float h = 1.f);

typedef std::array<Vector3, 8>	FrustumCorners;

void WorldSpaceFrustumCorners(FrustumCorners & corners
	, float near, float far, float projWidth, float projHeight
	, Matrix4x4 const & mtxInvView);

void SplitFrustum(std::vector<float> & splits, uint8_t numSplits
	, float near, float far, float splitWeight = 0.75f);

void CalcMeshNormal(std::vector<Vector3> & normals
	, std::vector<Vector3> const & vertices, std::vector<uint16_t> indices);

void CreateCuboid(std::vector<PosColorNormVertex> & out, Vector3 const & length, uint32_t color);

void CreateEllipsoid(std::vector<PosColorNormVertex> & out, float phi, float delta);

}
