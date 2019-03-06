#include "renderUtil.h"
#include "viewState.h"
#include "bx/math.h"
#include "vtxDecl.h"
#include "../examples/common/bgfx_utils.h"
#include <cmath>
#include "math.h"
#include <map>
#include "frameBuffer.h"

//#pragma optimize("", off)


namespace Ushuaia
{

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


void DrawScreenQuad(Shader const *pShader
	, uint64_t state, float x, float y, float w, float h)
{
	assert(4 == bgfx::getAvailTransientVertexBuffer(4, PosVertex::s_decl));
	bgfx::TransientVertexBuffer vb;
	bgfx::allocTransientVertexBuffer(&vb, 4, PosVertex::s_decl);

	MakeScreenQuad(vb.data, x, y, w, h);

	bgfx::setVertexBuffer(0, &vb);

	state |= BGFX_STATE_PT_TRISTRIP;
	bgfx::setState(state);

	bgfx::submit(FrameBuffer::ViewID(), pShader->Tech());
}


void WorldSpaceFrustumCorners(FrustumCorners & corners
	, float near, float far, float projWidth, float projHeight
	, Matrix4x4 const & mtxInvView)
{
	Vector2 const ns { near * projWidth, near * projHeight };
	Vector2 const fs { far * projWidth, far * projHeight };

	Vector3 const rawCorners[8] = {
		{ -ns.x,  ns.y, near },
		{  ns.x,  ns.y, near },
		{  ns.x, -ns.y, near },
		{ -ns.x, -ns.y, near },
		{ -fs.x,  fs.y, far },
		{  fs.x,  fs.y, far },
		{  fs.x, -fs.y, far },
		{ -fs.x, -fs.y, far },
	};

	for (uint8_t ii = 0; ii < 8; ++ii) {
		Mul(corners[ii], rawCorners[ii], mtxInvView);
	}
}


/**
* _splits = { near0, far0, near1, far1... nearN, farN }
* N = _numSplits
*/
void SplitFrustum(std::vector<float> & _splits, uint8_t _numSplits
	, float near, float far, float _splitWeight)
{
	float const l = _splitWeight;
	float const ratio = far / near;
	int8_t const numSlices = _numSplits * 2;
	_splits.resize(numSlices);
	float const numSlicesF = static_cast<float>(numSlices);

	_splits[0] = near;

	for (uint8_t nn = 2, ff = 1; nn < numSlices; nn += 2, ff += 2) {
		float si = float(ff) / numSlicesF;

		float const nearP = l * (near * bx::pow(ratio, si)) + (1 - l)*(near + (far - near)*si);
		_splits[nn] = nearP;	// near
		_splits[ff] = nearP * 1.005f;	// far from previous split
	}

	// last slice
	_splits[numSlices - 1] = far;
}


void CalcMeshNormal(Vector3 * normals, uint16_t numNorm
	, Vector3 const * vertices, uint16_t numVtx
	, uint16_t const * indices, uint32_t numIdx)
{
	::memset(normals, 0, sizeof(Vector3) * numNorm);

	for (uint32_t i = 0; i < numIdx; i += 3) {
		Vector3 const & v0 = vertices[indices[i]];
		Vector3 const & v1 = vertices[indices[i+1]];
		Vector3 const & v2 = vertices[indices[i+2]];
		Vector3 n = CalcNormal(v0, v1, v2);
		for (uint8_t j = 0; j < 3; ++j) {
			normals[indices[i]] += n;
		}
	}
	
	for (uint16_t i = 0; i < numVtx; ++i) {
		normals[i].Normalize();
	}
}


uint16_t FloatToUint16(float f)
{
	union {
		int16_t i;
		uint16_t ui;
	} v;
	v.i = static_cast<int16_t>(Clamp(f, -1.f, 1.f) * INT16_MAX);
	return v.ui;
}

}
