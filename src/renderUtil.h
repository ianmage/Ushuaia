#pragma once

#include "math.h"
#include <array>
#include <vector>


namespace Ushuaia
{

	void screenSpaceQuad(float _texW, float _texH,
		bool _origBottmLeft = true, float _w = 1.f, float _h = 1.f);

	typedef std::array<Vector3, 8>	FrustumCorners;

	void worldSpaceFrustumCorners(FrustumCorners & _corners,
		float _near, float _far, float _projWidth, float _projHeight,
		Matrix4x4 const & _mtxInvView);

	void splitFrustum(std::vector<float> & _splits, uint8_t _numSplits,
		float _near, float _far, float _splitWeight = 0.75f);

}
