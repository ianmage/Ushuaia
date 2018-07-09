#pragma once

#include "math.h"


namespace Ushuaia
{
	typedef TVec4<float>	Color4F;
	typedef uint32_t	ColorI;
	typedef TVec3<float>	Color3F;


	Color4F ConvertToColorF(uint32_t const iVal);

	uint32_t ConvertToColorI(Color4F const & c);
}
