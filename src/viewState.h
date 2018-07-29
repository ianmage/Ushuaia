#pragma once

#include "math.h"


namespace Ushuaia
{

struct RENDER_PASS
{
	enum : uint16_t {
		GEOMETRY_ID,

		GEO_NUM
	};
};


struct ViewState
{
	uint16_t width;
	uint16_t height;

	static float texelOffset;
};

extern ViewState g_viewState;

}
