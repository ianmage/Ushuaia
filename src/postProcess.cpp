#include "postProcess.h"
#include "viewState.h"
#include "renderUtil.h"

#pragma optimize("", off)


namespace Ushuaia
{

bgfx::VertexBufferHandle PostProcess::hVB_;


void PostProcess::Init()
{
	hVB_ = BGFX_INVALID_HANDLE;
}


void PostProcess::Fini()
{
	if (isValid(hVB_)) {
		bgfx::destroy(hVB_);
		hVB_ = BGFX_INVALID_HANDLE;
	}
}


void PostProcess::DrawFullScreen(Shader const *pShader)
{
#if 1
	assert(3 == bgfx::getAvailTransientVertexBuffer(3, PosVertex::s_decl));
	bgfx::TransientVertexBuffer vb;
	bgfx::allocTransientVertexBuffer(&vb, 3, PosVertex::s_decl);
	PosVertex *pVtx = (PosVertex*)vb.data;

	float const xMin = -3.f;
	float const yMax = 3.f;
	float const zz = 0.f;

	pVtx[0].pos.Set(xMin, -1.f, zz);
	pVtx[1].pos.Set(1.f, -1.f, zz);
	pVtx[2].pos.Set(1.f, yMax, zz);

	bgfx::setVertexBuffer(0, &vb);
#else
	assert(isValid(hVB_));
	bgfx::setVertexBuffer(0, hVB_);
#endif
	bgfx::submit(FrameBuffer::CurrFB()->ViewID(), pShader->Tech());
}

}
