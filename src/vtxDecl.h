#pragma once

#include "../../cpp_common/mathUtil.h"
#include <bgfx/bgfx.h>


namespace My3D
{

	struct PosNormTC0Vertex
	{
		Vector3 pos;
		uint32_t normal;
		Vector2 uv;

		static bgfx::VertexDecl s_decl;
	};

	struct PosVertex
	{
		Vector3 pos;

		static bgfx::VertexDecl s_decl;
	};

	struct PosColorTC0Vertex
	{
		Vector3 pos;
		uint32_t rgba;
		Vector2 uv;

		static bgfx::VertexDecl s_decl;
	};

	struct PosColorVertex
	{
		Vector3 pos;
		uint32_t abgr;

		static bgfx::VertexDecl s_decl;
	};


	void initVtxDecl();

}
