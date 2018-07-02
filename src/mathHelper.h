#pragma once

#include <bx/math.h>
#include "../../cpp_common/mathUtil.h"



namespace math
{

	inline void mtxSRT(Matrix4x4 & mtx, Vector3 const & s, Vector3 const & r, Vector3 const & t)
	{
		bx::mtxSRT(mtx.v, s.x, s.y, s.z, r.x, r.y, r.z, t.x, t.y, t.z);
	}

	inline void vec4MulMtxComp3(Vector4 & out, Vector4 const & in, Matrix4x4 const & mtx)
	{
		bx::vec3MulMtx(out.v, in.v, mtx.v);
		out.w = in.w;
	}

}
