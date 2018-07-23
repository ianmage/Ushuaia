#pragma once

#include "math.h"


namespace Ushuaia
{

	struct ViewState
	{
		uint16_t width;
		uint16_t height;

		static Vector2 texelHalf;
		static bool flipV;
		static bool originBottomLeft;
	};


	extern ViewState g_viewState;

}
