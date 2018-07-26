#pragma once

#include <bgfx/bgfx.h>


namespace Ushuaia
{

struct Shading
{
	static void Init();

	static void Fini();

	static void Update();

	static void Render();

	static void Lost();
	static void Reset();

	static bgfx::TextureHandle hDepthTex;
};

}
