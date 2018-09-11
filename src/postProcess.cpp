#include "postProcess.h"
#include "viewState.h"
#include "renderUtil.h"

#pragma optimize("", off)


namespace Ushuaia
{

uint16_t PostProcess::viewID_;
Matrix4x4 PostProcess::mtxOrtho_;
uint16_t PostProcess::viewWidth_;
uint16_t PostProcess::viewHeight_;
bgfx::VertexBufferHandle PostProcess::hVB_;


void PostProcess::Init()
{
	bgfx::Caps const * caps = bgfx::getCaps();
	bx::mtxOrtho(mtxOrtho_.v, 0.f, 1.f, 1.f, 0.f, 0.f, 100.f, 0.f, caps->homogeneousDepth);
	hVB_ = BGFX_INVALID_HANDLE;
}


void PostProcess::Fini()
{
	if (isValid(hVB_)) {
		bgfx::destroy(hVB_);
		hVB_ = BGFX_INVALID_HANDLE;
	}
}


void PostProcess::Update()
{
	viewID_ = RENDER_PASS::GEO_NUM;
}


void PostProcess::NewFrameBuf(FrameBuffer const *pFB, bool doClear)
{
	++viewID_;

	bgfx::FrameBufferHandle hFB = BGFX_INVALID_HANDLE;
	if (pFB) {
		hFB = pFB->Handle();
		viewWidth_ = pFB->Width();
		viewHeight_ = pFB->Height();
	}
	else {
		viewWidth_ = g_viewState.width;
		viewHeight_ = g_viewState.height;
	}

	bgfx::setViewRect(viewID_, 0, 0, viewWidth_, viewHeight_);
	bgfx::setViewFrameBuffer(viewID_, hFB);
	bgfx::setViewMode(viewID_, bgfx::ViewMode::Sequential);
	bgfx::setViewTransform(viewID_, nullptr, mtxOrtho_.v);
	if (doClear)
		bgfx::setViewClear(viewID_,
			BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 1.f, 0, 0);
}


static void CalcUvRect(uint16_t w, uint16_t h, TRect<float> & uvRect)
{
	Vector2 const texelHalf{
		ViewState::texelOffset / w,
		ViewState::texelOffset / h
	};

	uvRect.rMin = { -1.f + texelHalf.x, texelHalf.y };
	uvRect.rMax = { 1.f + texelHalf.x, 2.f + texelHalf.y };

	if (bgfx::getCaps()->originBottomLeft) {
		std::swap(uvRect.rMin.y, uvRect.rMax.y);
		uvRect.rMin.y -= 1.f;
		uvRect.rMax.y -= 1.f;
	}
}


void PostProcess::DrawFullScreen(Shader const *pShader)
{
	assert(viewWidth_ > 0 && viewHeight_ > 0);
#if 1
	assert(3 == bgfx::getAvailTransientVertexBuffer(3, PosTC0Vertex::s_decl));
	bgfx::TransientVertexBuffer vb;
	bgfx::allocTransientVertexBuffer(&vb, 3, PosTC0Vertex::s_decl);
	PosTC0Vertex *pVtx = (PosTC0Vertex*)vb.data;

	float const zz = 0.f;

	TRect<float> xyRect { { -1.f, 0.f }, { 1.f, 2.f } };
	TRect<float> uvRect;
	CalcUvRect(viewWidth_, viewHeight_, uvRect);
	Vector2 vVec, uv;

	pVtx[0].pos.Set(xyRect.rMin.x, xyRect.rMin.y, zz);
	uv = { uvRect.rMin.x, uvRect.rMin.y };
	ViewVecForReconstructPos(vVec, uv, Camera::pCurrent);
	pVtx[0].tc.Set(uv.x, uv.y, vVec.x, vVec.y);

	pVtx[1].pos.Set(xyRect.rMax.x, xyRect.rMin.y, zz);
	uv = { uvRect.rMax.x, uvRect.rMin.y };
	ViewVecForReconstructPos(vVec, uv, Camera::pCurrent);
	pVtx[1].tc.Set(uv.x, uv.y, vVec.x, vVec.y);

	pVtx[2].pos.Set(xyRect.rMax.x, xyRect.rMax.y, zz);
	uv = { uvRect.rMax.x, uvRect.rMax.y };
	ViewVecForReconstructPos(vVec, uv, Camera::pCurrent);
	pVtx[2].tc.Set(uv.x, uv.y, vVec.x, vVec.y);

	bgfx::setVertexBuffer(0, &vb);
#else
	assert(isValid(hVB_));
	bgfx::setVertexBuffer(0, hVB_);
#endif
	bgfx::submit(viewID_, pShader->Tech());
}

}
