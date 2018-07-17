#include "forwardRendering.h"
#include "viewState.h"
#include "resData.h"
#include "camera.h"
#include "bx/math.h"
#include "drawItem.h"

//#pragma optimize("", off)
#define TEST	0

namespace Ushuaia
{

	bgfx::ViewId const RENDERVIEW_DRAWSCENE_0 = 0;


	void ForwardRendering::Init()
	{
		
	}


	void ForwardRendering::Fini()
	{
		
	}


	void ForwardRendering::Update()
	{
		
	}


	void ForwardRendering::render()
	{
		bgfx::setViewRect(0, 0, 0, g_viewState.width, g_viewState.height);
		bgfx::setViewFrameBuffer(0, BGFX_INVALID_HANDLE);
#if TEST
		Vector3 at{ 0.f, 0.f, 0.f };
		Vector3 eye{ 0.f, 0.f, -35.f };
		Matrix4x4 view;
		bx::mtxLookAt(view.v, eye.v, at.v);

		float const camAspect = float(g_viewState.width) / float(g_viewState.height);
		bgfx::Caps const * caps = bgfx::getCaps();
		Matrix4x4 proj;
		bx::mtxProj(proj.v, 60.f, camAspect, 0.1f, 100.f, caps->homogeneousDepth);

		bgfx::setViewTransform(0, view.v, proj.v);

#else
		auto pCam = Camera::pCurrent;
		bgfx::setViewTransform(0, pCam->mtxView.v, pCam->mtxProj.v);
#endif

		bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH
			, 0x30303000, 1.f, 0);

		Light::Submit();
#if TEST
		uint64_t state = 0
			| BGFX_STATE_WRITE_R
			| BGFX_STATE_WRITE_G
			| BGFX_STATE_WRITE_B
			| BGFX_STATE_WRITE_A
			| BGFX_STATE_WRITE_Z
			| BGFX_STATE_DEPTH_TEST_LESS
			| BGFX_STATE_CULL_CW
			| BGFX_STATE_MSAA
			| UINT64_C(0)
			;
		bgfx::setState(state);
#endif
		uint64_t overrideSt0 = RenderState::s_val[0];
		uint64_t overrideSt1 = 0
			| BGFX_STATE_WRITE_RGB
			| BGFX_STATE_WRITE_A
			| BGFX_STATE_WRITE_Z
			| BGFX_STATE_DEPTH_TEST_LESS
			| BGFX_STATE_MSAA
		;

		//for (auto const & m : Entity::s_scnEnts)
		//{
		//	auto const & e = m.second;
		//	bgfx::setTransform(e->mtx.v);
		//	e->pModel->draw(0, overrideSt0, overrideSt1);
		//}
		//for (auto const & m : Entity::s_dynEnts)
		//{
		//	auto const & e = m.second;
		//	bgfx::setTransform(e->mtx.v);
		//	e->pModel->draw(0, overrideSt0, overrideSt1);
		//}
		DrawChannel::Gather();

		DrawChannel::DrawOpaque(0, overrideSt0, overrideSt1);

		DrawChannel::ClearAll();
	}

}
