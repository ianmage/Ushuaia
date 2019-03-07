#pragma once

#include "shader.h"
#include "vtxDecl.h"
#include "frameBuffer.h"


namespace Ushuaia
{

class PostProcessor
{
public:
	virtual void Render(Texture const *pSrcTex, FrameBuffer const *pOutFB) = 0;
};


struct PostProcess
{
public:
	static void Init();
	static void Fini();

	static void DrawFullScreen(Shader const *pShader);

	static void Add(PostProcessor *pProcessor) {
		processors_.push_back(pProcessor);
	}
	static void Del(PostProcessor const * pProcessor);
	static void Render(Texture const *pSrcTex, FrameBuffer const *pFB);

private:
	static std::vector<PostProcessor*> processors_;
};

}
