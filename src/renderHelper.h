#pragma once

#include "math.h"
#include <vector>


namespace Ushuaia
{

void CreateCuboid(std::vector<Vector3> & vtxOut, std::vector<uint16_t> & idxOut
	, std::vector<uint32_t> * normOut, Vector3 const & length);

void CreateSphere(std::vector<Vector3> & vtxOut, std::vector<uint16_t> & idxOut
	, std::vector<uint32_t> * normOut, uint8_t lod, float scale);


}
