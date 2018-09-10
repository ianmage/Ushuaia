#include "postProcess.h"
#include "viewState.h"

#pragma optimize("", off)


namespace Ushuaia
{

uint16_t PostProcess::viewID_;
Matrix4x4 PostProcess::mtxOrtho_;
bgfx::VertexBufferHandle PostProcess::hVB_;


void PostProcess::Init()
{
	bgfx::Caps const * caps = bgfx::getCaps();
	bx::mtxOrtho(mtxOrtho_.v, 0.f, 1.f, 1.f, 0.f, 0.f, 100.f, 0.f, caps->homogeneousDepth);
	hVB_ = BGFX_INVALID_HANDLE;
}


void PostProcess::Fini()
{
	if (isValid(hVB_))
		bgfx::destroy(hVB_);
}


void PostProcess::Update()
{
	viewID_ = RENDER_PASS::GEO_NUM;
}


void PostProcess::NewFrameBuf(bgfx::FrameBufferHandle hFB, bool doClear)
{
	++viewID_;
	bgfx::setViewRect(viewID_, 0, 0, g_viewState.width, g_viewState.height);
	bgfx::setViewFrameBuffer(viewID_, hFB);
	bgfx::setViewMode(viewID_, bgfx::ViewMode::Sequential);
	bgfx::setViewTransform(viewID_, nullptr, mtxOrtho_.v);
	if (doClear)
		bgfx::setViewClear(viewID_,
			BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 1.f, 0, 0);
}


void PostProcess::DrawFullScreen(Shader const *pShader)
{
#if 1
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

	bgfx::setVertexBuffer(0, &vb);
#else
	assert(isValid(hVB_));
	bgfx::setVertexBuffer(0, hVB_);
#endif
	bgfx::submit(viewID_, pShader->Tech());
}

}
