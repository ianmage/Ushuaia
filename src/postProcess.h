#pragma once

#include "shader.h"
#include "vtxDecl.h"


namespace Ushuaia
{

struct PostProcess
{
public:
	static uint16_t Next();

	static void Init();
	static void Reset();
	static void Fini();

	static void DrawFullScreen(bgfx::ViewId viewId, Shader const *pShader);

private:
	static uint16_t PASS_ID_;
	static bgfx::VertexBufferHandle hVB_;

public:
	static uint16_t const & PASS_ID;
};

}
