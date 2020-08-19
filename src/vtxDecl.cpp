#include "vtxDecl.h"


namespace Ushuaia
{

bgfx::VertexLayout PosNormTC0Vertex::s_vtxLayout;
bgfx::VertexLayout PosVertex::s_vtxLayout;
bgfx::VertexLayout PosNormVertex::s_vtxLayout;


void InitVtxDecl()
{
	PosNormTC0Vertex::s_vtxLayout.begin()
		.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
		.add(bgfx::Attrib::Normal, 4, bgfx::AttribType::Uint8, true, true)
		.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
		.end();


	PosVertex::s_vtxLayout.begin()
		.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
		.end();

	PosNormVertex::s_vtxLayout.begin()
		.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
		.add(bgfx::Attrib::Normal, 4, bgfx::AttribType::Uint8, true, true)
		.end();
}

}
