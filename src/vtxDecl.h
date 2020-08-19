#pragma once

#include "math.h"
#include <bgfx/bgfx.h>


namespace Ushuaia
{

struct PosNormTC0Vertex
{
	Vector3 pos;
	uint32_t normal;
	Vector2 tc;

	static bgfx::VertexLayout s_vtxLayout;
};

struct PosVertex
{
	Vector3 pos;

	static bgfx::VertexLayout s_vtxLayout;
};

struct PosNormVertex
{
	Vector3 pos;
	uint32_t normal;

	static bgfx::VertexLayout s_vtxLayout;
};


void InitVtxDecl();

}
