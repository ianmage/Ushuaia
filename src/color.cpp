#include "color.h"


Color4F convertToColorF(uint32_t const iVal)
{
	Color4F c;
	uint32_t iR = iVal >> 24;
	uint32_t iG = (iVal >> 16) & 0xFF;
	uint32_t iB = (iVal >> 8) & 0xFF;
	uint32_t iA = iVal & 0xFF;
	c.x = static_cast<float>(iR);
	c.y = static_cast<float>(iG);
	c.z = static_cast<float>(iB);
	c.w = static_cast<float>(iA);
	return std::move(c);
}


uint32_t convertToColorI(Color4F const & c)
{
	uint32_t const minI = 0;
	uint32_t const maxI = 255;
	uint32_t iR = clamp(static_cast<uint32_t>(c.x), minI, maxI);
	uint32_t iG = clamp(static_cast<uint32_t>(c.y), minI, maxI);
	uint32_t iB = clamp(static_cast<uint32_t>(c.z), minI, maxI);
	uint32_t iA = clamp(static_cast<uint32_t>(c.w), minI, maxI);
	return (iR << 24) & (iG << 16) & (iB << 8) & iA;
}