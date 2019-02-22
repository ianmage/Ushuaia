#pragma once

#include <bgfx/bgfx.h>
#include "texture.h"
#include "frameBuffer.h"


namespace Ushuaia
{

struct HDR
{
	static void Init();

	static void Fini();

	static void Update();

	static void Render(Texture const & tex, FrameBuffer const * pOutFB);

	static void Lost();
	static void Reset();
};

}
