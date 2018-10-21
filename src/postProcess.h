#pragma once

#include "shader.h"
#include "vtxDecl.h"
#include "frameBuffer.h"


namespace Ushuaia
{

struct PostProcess
{
public:
	static void Init();
	static void Fini();

	static void DrawFullScreen(Shader const *pShader);

private:
	static bgfx::VertexBufferHandle hVB_;
};

}
