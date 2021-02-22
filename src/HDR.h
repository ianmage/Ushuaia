#pragma once

#include "PostProcess.h"


namespace Ushuaia
{

class HDR : public PostProcessor
{
public :
	static HDR& Instance() { return s_instance; }

	void Update();
	void Render(Texture const *pTex, FrameBuffer const *pFB) override;

protected :
	bool Init() override;
	void Fini() override;

private :
	static HDR s_instance;

	Shader *pTech_;
};

}
