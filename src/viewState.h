#pragma once

#include "math.h"


namespace Ushuaia
{

struct RENDER_PASS
{
public:
	enum : uint16_t {
		GEOMETRY_ID,

		GEO_NUM
	};

	static uint16_t NextPP();

private:
	static uint16_t PP_ID_;

public:
	static uint16_t const & PP_ID;
};


struct ViewState
{
	uint16_t width;
	uint16_t height;

	static float texelOffset;
};

extern ViewState g_viewState;

}
