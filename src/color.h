#pragma once

#include "../../cpp_common/mathUtil.h"


typedef TVec4<float>	Color4F;
typedef uint32_t	ColorI;
typedef TVec3<float>	Color3F;


Color4F convertToColorF(uint32_t const iVal);

uint32_t convertToColorI(Color4F const & c);
