#pragma once

#include "../../cpp_common/mathUtil.h"


namespace My3D
{

	struct ViewState
	{
		uint16_t width;
		uint16_t height;

		static Vector2 texelHalf;
		static bool flipV;
	};


	extern ViewState g_viewState;

}
