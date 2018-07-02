#pragma once

#include "bx/math.h"
#include "../../cpp_common/mathUtil.h"



namespace math
{

	void mtxSRT(Matrix4x4 & mtx, Vector3 const & s, Vector3 const & r, Vector3 const & t)
	{
		bx::mtxSRT(mtx.v, s.x, s.y, s.z, r.x, r.y, r.z, t.x, t.y, t.z);
	}

}
