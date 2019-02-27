#pragma once

#include "shader.h"
#include "framebuffer.h"


namespace Ushuaia
{

class GaussianBlur
{
public :
	static void Render(Texture const & srcTex, FrameBuffer const *pOutFB);

private :
	static bool Init();

	static Shader *pTechX, *pTechY;
};

}
