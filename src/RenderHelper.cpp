#include "RenderHelper.h"
#include <cmath>
#include <map>
#include <array>
#include <bx/math.h>
#include "Math.h"
#include "RenderUtil.h"
#include "../examples/common/bgfx_utils.h"


namespace Ushuaia
{

void CreateCuboid(std::vector<Vector3> & vtxOut, std::vector<uint16_t> & idxOut
	, std::vector<uint32_t> * normOut, Vector3 const & length)
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

	vtxOut.assign(pos.begin(), pos.end());

	idxOut.assign(idx.begin(), idx.end());

	if (normOut) {
		std::array<Vector3, 8> normals;
		CalcMeshNormal(normals.data(), (uint16_t)normals.size(),
			pos.data(), (uint16_t)pos.size(), idx.data(), (uint32_t)idx.size());

		normOut->resize(8);
		for (uint8_t i = 0; i < 8; ++i) {
			auto const & n = normals[i];
			(*normOut)[i] = ::encodeNormalRgba8(n.x, n.y, n.z);
		}
	}
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


void CreateSphere(std::vector<Vector3> & vtxOut, std::vector<uint16_t> & idxOut
	, std::vector<uint32_t> * normOut, uint8_t lod, float scale)
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

	if (normOut) {
		uint16_t const vtxCnt = (uint16_t)vtxPos.size();
		normOut->resize(vtxCnt);
		for (uint16_t i = 0; i < vtxCnt; ++i) {
			auto const & vp = vtxPos[i];
			(*normOut)[i] = ::encodeNormalRgba8(vp.x, vp.y, vp.z);
		}
	}

	if (scale != 1.f) {
		vtxOut.clear();
		for (auto const & vp : vtxPos)
			vtxOut.emplace_back(vp * scale);
	}
	else {
		std::swap(vtxOut, vertices[curr]);
	}
}

}
