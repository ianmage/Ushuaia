#include "renderUtil.h"
#include "viewState.h"
#include "bx/math.h"
#include "vtxDecl.h"
#include "../examples/common/bgfx_utils.h"
#include <cmath>
#include "math.h"
#include <map>

#pragma optimize("", off)


namespace Ushuaia
{

void ScreenSpaceQuad(float texW, float texH, float w, float h)
{
	if (3 == bgfx::getAvailTransientVertexBuffer(3, PosTC0Vertex::s_decl))
	{
		bgfx::TransientVertexBuffer vb;
		bgfx::allocTransientVertexBuffer(&vb, 3, PosTC0Vertex::s_decl);
		PosTC0Vertex *pVert = (PosTC0Vertex*)vb.data;

		float const zz = 0.f;

		TRect<float> xyRect{ { -w, 0.f }, { w, h*2.f } };

		Vector2 const texelHalf{ ViewState::texelOffset / texW,
			ViewState::texelOffset / texH };

		TRect<float> uvRect{ { -1.f + texelHalf.x, texelHalf.y }
			, { 1.f + texelHalf.x, 2.f + texelHalf.y } };

		if (bgfx::getCaps()->originBottomLeft)
		{
			std::swap(uvRect.rMin.y, uvRect.rMax.y);
			uvRect.rMin.y -= 1.f;
			uvRect.rMax.y -= 1.f;
		}

		pVert[0].pos.Set(xyRect.rMin.x, xyRect.rMin.y, zz);
		pVert[0].tc.Set(uvRect.rMin.x, uvRect.rMin.y);

		pVert[1].pos.Set(xyRect.rMax.x, xyRect.rMin.y, zz);
		pVert[1].tc.Set(uvRect.rMax.x, uvRect.rMin.y);

		pVert[2].pos.Set(xyRect.rMax.x, xyRect.rMax.y, zz);
		pVert[2].tc.Set(uvRect.rMax.x, uvRect.rMax.y);

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


void CreateCuboid(std::vector<PosNormVertex> & vtxOut, std::vector<uint16_t> & idxOut
	, Vector3 const & length)
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
	CalcMeshNormal(normals.data(), (uint16_t)normals.size(),
		pos.data(), (uint16_t)pos.size(), idx.data(), (uint32_t)idx.size());

	vtxOut.resize(8);
	for (uint8_t i = 0; i < 8; ++i) {
		vtxOut[i].pos = pos[i];
		auto const & n = normals[i];
		vtxOut[i].normal = encodeNormalRgba8(n.x, n.y, n.z);
	}

	idxOut.assign(idx.begin(), idx.end());
}


static void Icosahedron(std::vector<Vector3> & vertices, std::vector<uint16_t> & indices)
{
	//float const s = 1.f;
	//float const t = (1.f + ::sqrtf(5.f)) * 0.5f;
	float const s = 0.5257311f;
	float const t = 0.8506508f;

	std::array<Vector3, 12> const vtx{ {
		{  -s,   t, 0.f },
		{   s,   t, 0.f },
		{  -s,  -t, 0.f },
		{   s,  -t, 0.f },
		{ 0.f,  -s,   t },
		{ 0.f,   s,   t },
		{ 0.f,  -s,  -t },
		{ 0.f,   s,  -t },
		{   t, 0.f,  -s },
		{   t, 0.f,   s },
		{  -t, 0.f,  -s },
		{  -t, 0.f,   s },
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


struct Edge
{
	uint16_t i0, i1;

	Edge(uint16_t _i0, uint16_t _i1) {
		if (_i0 < _i1) {
			i0 = _i0; i1 = _i1;
		}
		else {
			i0 = _i1; i1 = _i0;
		}
	}

	bool operator < (Edge const & other) const {
		return i0 < other.i0 ||
			(i0 == other.i0 && i1 < other.i1);
	}
};


static uint16_t SubdivideEdge(uint16_t i0, uint16_t i1, Vector3 const & v0, Vector3 const & v1,
	std::vector<Vector3> & vertices, std::map<Edge, uint16_t> & divisions)
{
	Edge const edge(i0, i1);
	auto itr = divisions.find(edge);
	if (itr != divisions.end())
		return itr->second;
	uint16_t const vtxCnt = static_cast<uint16_t>(vertices.size());
	divisions.emplace(edge, vtxCnt);
	vertices.emplace_back( (v0 + v1) * 0.5f );
	vertices.back().Normalize();
	return vtxCnt;
}


static void SubdivSphere(
	std::vector<Vector3> & vtxOut, std::vector<uint16_t> & idxOut,
	std::vector<Vector3> const & vtxIn, std::vector<uint16_t> const & idxIn)
{
	vtxOut = vtxIn;
	std::map<Edge, uint16_t> divisions;

	uint32_t const triangleCnt = (uint32_t)idxIn.size() / 3;
	for (uint16_t f = 0; f < triangleCnt; ++f) {
		uint16_t const i0 = idxIn[f * 3];
		uint16_t const i1 = idxIn[f * 3 + 1];
		uint16_t const i2 = idxIn[f * 3 + 2];

		Vector3 const & v0 = vtxIn[i0];
		Vector3 const & v1 = vtxIn[i1];
		Vector3 const & v2 = vtxIn[i2];

		uint16_t const i3 = SubdivideEdge(i0, i1, v0, v1, vtxOut, divisions);
		uint16_t const i4 = SubdivideEdge(i1, i2, v1, v2, vtxOut, divisions);
		uint16_t const i5 = SubdivideEdge(i2, i0, v2, v0, vtxOut, divisions);

		idxOut.push_back(i0); idxOut.push_back(i3); idxOut.push_back(i5);
		idxOut.push_back(i3); idxOut.push_back(i1); idxOut.push_back(i4);
		idxOut.push_back(i4); idxOut.push_back(i2); idxOut.push_back(i5);
		idxOut.push_back(i3); idxOut.push_back(i4); idxOut.push_back(i5);
	}
}


void CreateSphere(std::vector<PosNormVertex> & vtxOut, std::vector<uint16_t> & idxOut
	, uint8_t lod, float scale)
{
	std::vector<Vector3> vertices[2];
	std::vector<uint16_t> indices[2];
	uint8_t curr = 0;
	Icosahedron(vertices[0], indices[0]);
	for (uint8_t d = 0; d < lod; ++d) {
		uint8_t prev = curr;
		curr = (curr + 1) % 2;

		vertices[curr].clear();
		indices[curr].clear();
		SubdivSphere(vertices[curr], indices[curr], vertices[prev], indices[prev]);
	}

	std::swap(idxOut, indices[curr]);

	auto const & vtxPos = vertices[curr];
	uint16_t const vtxCnt = (uint16_t)vtxPos.size();
	vtxOut.resize(vtxCnt);
	for (uint16_t i = 0; i < vtxCnt; ++i) {
		auto & vo = vtxOut[i];
		vo.pos = vtxPos[i];
		vo.normal = ::encodeNormalRgba8(vo.pos.x, vo.pos.y, vo.pos.z);
		vo.pos *= scale;
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
