#include "vtxDecl.h"


namespace My3D
{
	bgfx::VertexDecl PosNormTC0Vertex::s_decl;
	bgfx::VertexDecl PosVertex::s_decl;
	bgfx::VertexDecl PosColorTC0Vertex::s_decl;
	bgfx::VertexDecl PosColorVertex::s_decl;
	
	void initVtxDecl()
	{
		PosNormTC0Vertex::s_decl.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Normal, 4, bgfx::AttribType::Uint8, true, true)
			.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
			.end();


		PosVertex::s_decl.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.end();


		PosColorTC0Vertex::s_decl.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, false, true)
			.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
			.end();

		PosColorVertex::s_decl.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, false, true)
			.end();
	}
}
