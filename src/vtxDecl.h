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

	static bgfx::VertexDecl s_decl;
};

struct PosVertex
{
	Vector3 pos;

	static bgfx::VertexDecl s_decl;
};

struct PosTC0Vertex
{
	Vector3 pos;
	Vector4 tc;

	static bgfx::VertexDecl s_decl;
};

struct PosNormVertex
{
	Vector3 pos;
	uint32_t normal;

	static bgfx::VertexDecl s_decl;
};


void InitVtxDecl();

}
