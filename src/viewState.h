#pragma once

#include "math.h"


namespace Ushuaia
{

struct ViewState
{
	uint16_t width;
	uint16_t height;

	static float texelOffset;
};


extern ViewState g_viewState;

}
