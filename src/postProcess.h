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

	static void Update();
	static void NewFrameBuf(FrameBuffer const *pFB, bool doClear);

	static void DrawFullScreen(Shader const *pShader);

	static uint16_t ViewID() { return viewID_; };

private:
	static uint16_t viewID_;
	static Matrix4x4 mtxOrtho_;

	static uint16_t viewWidth_, viewHeight_;

	static bgfx::VertexBufferHandle hVB_;
};

}
