#include "PostProcess.h"
#include "ViewState.h"
#include "RenderUtil.h"

#pragma optimize("", off)


namespace Ushuaia
{

decltype(PostProcessor::instances_) PostProcessor::instances_;


PostProcessor::PostProcessor()
{
	instances_.push_back(this);
}

PostProcessor::~PostProcessor()
{
	for (int i = 0; i < instances_.size(); ++i) {
		if (instances_[i] == this)
			instances_.erase(instances_.begin() + i);
	}
}


void PostProcessor::FiniAll()
{
	for (auto ppp : instances_)
		ppp->Fini();
}


decltype(PostProcess::processors_) PostProcess::processors_;


void PostProcess::DrawFullScreen(Shader const *pShader)
{
	assert(3 == bgfx::getAvailTransientVertexBuffer(3, PosVertex::s_vtxLayout));
	bgfx::TransientVertexBuffer vb;
	bgfx::allocTransientVertexBuffer(&vb, 3, PosVertex::s_vtxLayout);
	PosVertex *pVtx = (PosVertex*)vb.data;

	float const xMin = -3.f;
	float const yMax = 3.f;
	float const zz = 0.f;

	pVtx[0].pos.Set(xMin, -1.f, zz);
	pVtx[1].pos.Set(1.f, -1.f, zz);
	pVtx[2].pos.Set(1.f, yMax, zz);

	bgfx::setVertexBuffer(0, &vb);

	bgfx::submit(FrameBuffer::ViewID(), pShader->Tech());
}


void PostProcess::Del(PostProcessor const * pProcessor)
{
	int i = 0, j = 0;
	size_t const len = processors_.size();
	for (i = 0; i < len && processors_[i] != pProcessor; ++i);
	while (true) {
		for (j = i + 1; j < len && processors_[j] == pProcessor; ++j);
		if (j >= len)
			break;
		do {
			processors_[i++] = processors_[j++];
		} while (j < len && processors_[j] != pProcessor);
		i = j;
	}
	processors_.erase(processors_.begin()+i, processors_.end());
}


void PostProcess::Render(Texture const *pSrcTex, FrameBuffer const *pFB)
{
	size_t const num = processors_.size();

	if (num <= 1) {
		// 0 should do BltCopy
		if (num == 0)
			Copy(pSrcTex, pFB, BGFX_STATE_WRITE_RGB, false);
		else
			processors_[0]->Render(pSrcTex, pFB);
		return;
	}

	int ppIdx = num % 2;

	FrameBuffer const * pPingPongFB[2];
	pPingPongFB[0] = FrameBuffer::CheckOut(pFB->Width(), pFB->Height(), pFB->pTex()->Format());
	pPingPongFB[1] = pFB;

	Texture const *pTex = pSrcTex;

	for (int i = 0; i < num; ++i) {
		processors_[i]->Render(pTex, pPingPongFB[ppIdx]);

		pTex = pPingPongFB[ppIdx]->pTex();
		ppIdx = 1 - ppIdx;
	}

	FrameBuffer::CheckIn(pPingPongFB[0]);
}

}
