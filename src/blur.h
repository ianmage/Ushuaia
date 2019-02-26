#pragma once

#include "shader.h"


namespace Ushuaia
{

class GaussianBlur
{
public :
	static void Render(Texture const & srcTex, FrameBuffer *pOutFB);

private :
	static bool Init();

	static Shader *pXTech, *pYTech;
};

}
