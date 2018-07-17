#pragma once


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
};

}
