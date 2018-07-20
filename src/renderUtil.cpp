#include "renderUtil.h"
#include "viewState.h"
#include "bx/math.h"
#include "vtxDecl.h"
#include "../examples/common/bgfx_utils.h"


namespace Ushuaia
{

void ScreenSpaceQuad(float texW, float texH, bool origBottomLeft, float w, float h)
{
	if (3 == bgfx::getAvailTransientVertexBuffer(3, PosColorTC0Vertex::s_decl))
	{
		bgfx::TransientVertexBuffer vb;
		bgfx::allocTransientVertexBuffer(&vb, 3, PosColorTC0Vertex::s_decl);
		PosColorTC0Vertex *pVert = (PosColorTC0Vertex*)vb.data;

		float const zz = 0.f;

		TRect<float> xyRect{ { -w, 0.f }, { w*2.f, h*2.f } };

		Vector2 const texelHalf{ ViewState::texelHalf.x / texW,
			ViewState::texelHalf.y / texH };

		TRect<float> uvRect{ { -1.f + ViewState::texelHalf.x, ViewState::texelHalf.y }, { 2.f, 2.f } };

		if (origBottomLeft)
		{
			std::swap(uvRect.rMin.y, uvRect.rMax.y);
			uvRect.rMin.y -= 1.f;
			uvRect.rMax.y -= 1.f;
		}

		pVert[0].pos.Set(xyRect.rMin.x, xyRect.rMin.y, zz);
		pVert[0].rgba = 0xffffffff;
		pVert[0].uv.Set(uvRect.rMin.x, uvRect.rMin.y);

		pVert[1].pos.Set(xyRect.rMax.x, xyRect.rMin.y, zz);
		pVert[1].rgba = 0xffffffff;
		pVert[1].uv.Set(uvRect.rMax.x, uvRect.rMin.y);

		pVert[2].pos.Set(xyRect.rMax.x, xyRect.rMax.y, zz);
		pVert[2].rgba = 0xffffffff;
		pVert[2].uv.Set(uvRect.rMax.x, uvRect.rMax.y);

		bgfx::setVertexBuffer(0, &vb);
	}
}


void WorldSpaceFrustumCorners(FrustumCorners & corners
	, float near, float far, float projWidth, float projHeight
	, Matrix4x4 const & mtxInvView)
{
	Vector2 const ns{ near * projWidth, near * projHeight };
	Vector2 const fs{ far * projWidth, far * projHeight };

	float const rawCorners[8][3] = {
		{ -ns.x,  ns.y, near },
		{ ns.x,  ns.y, near },
		{ ns.x, -ns.y, near },
		{ -ns.x, -ns.y, near },
		{ -fs.x,  fs.y, far },
		{ fs.x,  fs.y, far },
		{ fs.x, -fs.y, far },
		{ -fs.x, -fs.y, far },
	};

	for (uint8_t ii = 0; ii < 8; ++ii)
	{
		bx::vec3MulMtx(corners[ii].v, (float*)&rawCorners[ii], mtxInvView.v);
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

	for (uint8_t nn = 2, ff = 1; nn < numSlices; nn += 2, ff += 2)
	{
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
		Vector3 n;
		Vector3 const & v0 = vertices[indices[i]];
		Vector3 const & v1 = vertices[indices[i+1]];
		Vector3 const & v2 = vertices[indices[i+2]];
		bx::calcNormal(n.v, v0.v, v1.v, v2.v);
		for (uint8_t j = 0; j < 3; ++j) {
			normals[indices[i]] += n;
		}
	}
	
	for (uint16_t i = 0; i < numVtx; ++i) {
		normals[i].Normalize();
	}
}


void CreateCuboid(std::vector<PosColorNormVertex> & out, Vector3 const & length, uint32_t color)
{
	std::array<Vector3, 8> const pos{ {
		{ -length.x,  length.y,  length.z },
		{  length.x,  length.y,  length.z },
		{ -length.x, -length.y,  length.z },
		{  length.x, -length.y,  length.z },
		{ -length.x,  length.y, -length.z },
		{  length.x,  length.y, -length.z },
		{ -length.x, -length.y, -length.z },
		{  length.x, -length.y, -length.z },
	} };

	std::array<uint16_t, 36> const idx{ {
		0, 1, 2, // 0
		1, 3, 2,
		4, 6, 5, // 2
		5, 6, 7,
		0, 2, 4, // 4
		4, 2, 6,
		1, 5, 3, // 6
		5, 7, 3,
		0, 4, 1, // 8
		4, 5, 1,
		2, 3, 6, // 10
		6, 3, 7,
	} };

	std::array<Vector3, 8> normals;
	CalcMeshNormal(normals.data(), normals.size(), pos.data(), pos.size(), idx.data(), idx.size());

	out.resize(8);
	for (uint8_t i = 0; i < 8; ++i) {
		out[i].pos = pos[i];
		auto const & n = normals[i];
		out[i].normal = encodeNormalRgba8(n.x, n.y, n.z);
		out[i].color = color;
	}
}


static void Icosahedron(std::vector<Vector3> & vertices, std::vector<uint16_t> & indices)
{
	float const t = (1.f + std::sqrtf(5.f)) * 0.5f;

	std::array<Vector3, 12> const vtx{ {
		{ -1.f,    t,  0.f },
		{ 1.f,    t,  0.f },
		{ -1.f,   -t,  0.f },
		{ 1.f,   -t,  0.f },
		{ 0.f, -1.f,    t },
		{ 0.f,  1.f,    t },
		{ 0.f, -1.f,   -t },
		{ 0.f,  1.f,   -t },
		{ t,  0.f, -1.f },
		{ t,  0.f,  1.f },
		{ -t,  0.f, -1.f },
		{ -t,  0.f,  1.f },
	} };

	std::array<uint16_t, 60> const idx{ {
		0, 11,  5,
		0,  5,  1,
		0,  1,  7,
		0,  7, 10,
		0, 10, 11,
		1,  5,  9,
		5, 11,  4,
		11, 10,  2,
		10,  7,  6,
		7,  1,  8,
		3,  9,  4,
		3,  4,  2,
		3,  2,  6,
		3,  6,  8,
		3,  8,  9,
		4,  9,  5,
		2,  4, 11,
		6,  2, 10,
		8,  6,  7,
		9,  8,  1,
	} };

	vertices.assign(vtx.begin(), vtx.end());
	indices.assign(idx.begin(), idx.end());
}


static void subdivSphere(
	std::vector<Vector3> const & vtxIn, std::vector<uint16_t> const & idxIn
	, std::vector<Vector3> & vtxOut, std::vector<uint16_t> & idxOut)
{

}


void CreateSphere(std::vector<PosColorNormVertex> & out, float phi, float delta)
{
	
}

}
