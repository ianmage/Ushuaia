#pragma once

#include "Shader.h"
#include "VtxDecl.h"
#include "FrameBuffer.h"


namespace Ushuaia
{

class PostProcessor
{
public :
	virtual void Render(Texture const *pSrcTex, FrameBuffer const *pOutFB) = 0;
	~PostProcessor();

	static void FiniAll();

protected :
	PostProcessor();
	virtual bool Init() = 0;
	virtual void Fini() = 0;

private :
	PostProcessor(PostProcessor const &) = delete;

	static std::vector<PostProcessor*> instances_;
};


struct PostProcess
{
public :
	static void DrawFullScreen(Shader const *pShader);

	static void Add(PostProcessor *pProcessor) {
		processors_.push_back(pProcessor);
	}
	static void Del(PostProcessor const * pProcessor);
	static void Render(Texture const *pSrcTex, FrameBuffer const *pFB);

private :
	static std::vector<PostProcessor*> processors_;
};

}
