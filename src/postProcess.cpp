#include "postProcess.h"
#include "viewState.h"

#pragma optimize("", off)


namespace Ushuaia
{

uint16_t PostProcess::PASS_ID_ = RENDER_PASS::GEO_NUM;
uint16_t const & PostProcess::PASS_ID = PostProcess::PASS_ID_;
bgfx::VertexBufferHandle PostProcess::hVB_;

uint16_t PostProcess::Next()
{
	PASS_ID_ = ((PASS_ID_ - RENDER_PASS::GEO_NUM) + 1) % 2 + RENDER_PASS::GEO_NUM;
	return PASS_ID_;
}


void PostProcess::Init()
{
	hVB_ = BGFX_INVALID_HANDLE;
	Reset();
}


void PostProcess::Fini()
{
	if (isValid(hVB_))
		bgfx::destroy(hVB_);
}

#ifdef __APPLE__
static uint8_t vData[3 * sizeof(PosTC0Vertex)];
#endif
void PostProcess::Reset()
{
	if (isValid(hVB_))
		bgfx::destroy(hVB_);

	bgfx::Memory const * mem = bgfx::alloc(3 * PosTC0Vertex::s_decl.getStride());
	PosTC0Vertex *pVtx = (PosTC0Vertex*)mem->data;

	float const zz = 0.f;

	TRect<float> xyRect { { -1.f, 0.f }, { 1.f, 2.f } };

	Vector2 const texelHalf {
		ViewState::texelOffset / g_viewState.width,
		ViewState::texelOffset / g_viewState.height
	};

	TRect<float> uvRect {
		{ -1.f + texelHalf.x, texelHalf.y },
		{ 1.f + texelHalf.x, 2.f + texelHalf.y }
	};

	if (bgfx::getCaps()->originBottomLeft) {
		std::swap(uvRect.rMin.y, uvRect.rMax.y);
		uvRect.rMin.y -= 1.f;
		uvRect.rMax.y -= 1.f;
	}

	pVtx[0].pos.Set(xyRect.rMin.x, xyRect.rMin.y, zz);
	pVtx[0].tc.Set(uvRect.rMin.x, uvRect.rMin.y);

	pVtx[1].pos.Set(xyRect.rMax.x, xyRect.rMin.y, zz);
	pVtx[1].tc.Set(uvRect.rMax.x, uvRect.rMin.y);

	pVtx[2].pos.Set(xyRect.rMax.x, xyRect.rMax.y, zz);
	pVtx[2].tc.Set(uvRect.rMax.x, uvRect.rMax.y);
#ifdef __APPLE__
	::memcpy(vData, mem->data, mem->size);
#endif
	hVB_ = bgfx::createVertexBuffer(mem, PosTC0Vertex::s_decl);
}


void PostProcess::DrawFullScreen(bgfx::ViewId viewId, Shader const *pShader)
{
#if 0
	assert(3 == bgfx::getAvailTransientVertexBuffer(3, PosTC0Vertex::s_decl));
	bgfx::TransientVertexBuffer vb;
	bgfx::allocTransientVertexBuffer(&vb, 3, PosTC0Vertex::s_decl);
	PosTC0Vertex *pVtx = (PosTC0Vertex*)vb.data;

	float const zz = 0.f;

	TRect<float> xyRect { { -1.f, 0.f }, { 1.f, 2.f } };

	Vector2 const texelHalf {
		ViewState::texelOffset / g_viewState.width,
		ViewState::texelOffset / g_viewState.height
	};

	TRect<float> uvRect{
		{ -1.f + texelHalf.x, texelHalf.y },
		{ 1.f + texelHalf.x, 2.f + texelHalf.y }
	};

	if (bgfx::getCaps()->originBottomLeft)
	{
		std::swap(uvRect.rMin.y, uvRect.rMax.y);
		uvRect.rMin.y -= 1.f;
		uvRect.rMax.y -= 1.f;
	}

	pVtx[0].pos.Set(xyRect.rMin.x, xyRect.rMin.y, zz);
	pVtx[0].tc.Set(uvRect.rMin.x, uvRect.rMin.y);

	pVtx[1].pos.Set(xyRect.rMax.x, xyRect.rMin.y, zz);
	pVtx[1].tc.Set(uvRect.rMax.x, uvRect.rMin.y);

	pVtx[2].pos.Set(xyRect.rMax.x, xyRect.rMax.y, zz);
	pVtx[2].tc.Set(uvRect.rMax.x, uvRect.rMax.y);
#ifdef __APPLE__
	assert(sizeof(vData) == vb.size);
	for (uint8_t ii = 0; ii < vb.size; ++ii) {
		assert(vb.data[ii] == vData[ii]);
	}
#endif
	bgfx::setVertexBuffer(0, &vb);
#else
	assert(isValid(hVB_));
	bgfx::setVertexBuffer(0, hVB_);
#endif
	bgfx::submit(viewId, pShader->Tech());
}

}
