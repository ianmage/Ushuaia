#include "vtxDecl.h"


namespace Ushuaia
{

bgfx::VertexDecl PosNormTC0Vertex::s_decl;
bgfx::VertexDecl PosVertex::s_decl;
bgfx::VertexDecl PosColorTC0Vertex::s_decl;
bgfx::VertexDecl PosNormVertex::s_decl;


void InitVtxDecl()
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

	PosNormVertex::s_decl.begin()
		.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
		.add(bgfx::Attrib::Normal, 4, bgfx::AttribType::Uint8, true, true)
		.end();
}

}
