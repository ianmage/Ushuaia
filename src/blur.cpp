#include "renderUtil.h"
#include "viewState.h"
#include "bx/math.h"
#include "vtxDecl.h"
#include "../examples/common/bgfx_utils.h"
#include <cmath>
#include "math.h"
#include <map>
#include "frameBuffer.h"

#pragma optimize("", off)


namespace Ushuaia
{

static void MakeScreenQuad(void * pVerts,
	float x, float y, float w, float h)
{
	x = x * 2.f - 1.f;
	y = 1.f - y * 2.f;
	w *= 2.f;
	h *= 2.f;
	float const zz = 0.f;

	TRect<float> xyRect { { x, y-h }, { x+w, y } };

	PosVertex *pVtx = reinterpret_cast<PosVertex*>(pVerts);

	pVtx[0].pos.Set(xyRect.rMin.x, xyRect.rMax.y, zz);
	pVtx[1].pos.Set(xyRect.rMax.x, xyRect.rMax.y, zz);
	pVtx[2].pos.Set(xyRect.rMin.x, xyRect.rMin.y, zz);
	pVtx[3].pos.Set(xyRect.rMax.x, xyRect.rMin.y, zz);
}


void GaussianBlur(Shader const *pShader
	, uint64_t state, float x, float y, float w, float h)
{
	assert(4 == bgfx::getAvailTransientVertexBuffer(4, PosVertex::s_decl));
	bgfx::TransientVertexBuffer vb;
	bgfx::allocTransientVertexBuffer(&vb, 4, PosVertex::s_decl);

	MakeScreenQuad(vb.data, x, y, w, h);

	bgfx::setVertexBuffer(0, &vb);

	state |= BGFX_STATE_PT_TRISTRIP;
	bgfx::setState(state);

	bgfx::submit(FrameBuffer::CurrFB()->ViewID(), pShader->Tech());
}

}
