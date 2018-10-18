#include "forwardRendering.h"
#include "viewState.h"
#include "script.h"
#include "camera.h"
#include "bx/math.h"
#include "drawItem.h"

//#pragma optimize("", off)
#define TEST	0

namespace Ushuaia
{

static bgfx::UniformHandle uhLightsCnt;
static bgfx::UniformHandle uhPointColor;
static bgfx::UniformHandle uhPointPos;	// w for spot Outer
static bgfx::UniformHandle uhPointAttnOuter;
static bgfx::UniformHandle uhSpotDirInner;


uint8_t const MAX_POINT_LIGHT = 6;
uint8_t const MAX_SPOT_LIGHT = 2;


static std::vector<Color4F> s_lightColorBuf;
static std::vector<Vector4> s_lightPosBuf;
static std::vector<Vector4> s_lightAttnOuterBuf;
static std::vector<Vector4> s_spotDirInnerBuf;
static Vector4 s_lightsCnt;


void ForwardRendering::Init()
{
	uhLightsCnt = bgfx::createUniform("PV_lightsCnt", bgfx::UniformType::Vec4);
	uhPointColor = bgfx::createUniform("PV_lightColor", bgfx::UniformType::Vec4, MAX_POINT_LIGHT + MAX_SPOT_LIGHT);
	uhPointPos = bgfx::createUniform("PV_lightPos", bgfx::UniformType::Vec4, MAX_POINT_LIGHT + MAX_SPOT_LIGHT);
	uhPointAttnOuter = bgfx::createUniform("PV_lightAttnOuter", bgfx::UniformType::Vec4, MAX_POINT_LIGHT + MAX_SPOT_LIGHT);
	uhSpotDirInner = bgfx::createUniform("PV_lightSpotDirInner", bgfx::UniformType::Vec4, MAX_SPOT_LIGHT);
}


void ForwardRendering::Fini()
{
	bgfx::destroy(uhLightsCnt);
	bgfx::destroy(uhPointColor);
	bgfx::destroy(uhPointPos);
	bgfx::destroy(uhPointAttnOuter);
	bgfx::destroy(uhSpotDirInner);
}


void ForwardRendering::Update()
{
	s_lightsCnt.Set(0, 0, 0, 0);
	s_lightColorBuf.clear();
	s_lightPosBuf.clear();
	s_lightAttnOuterBuf.clear();
	s_spotDirInnerBuf.clear();

	s_lightsCnt.x = s_lightsCnt.y = 1.f;

	uint8_t const plCnt = std::min(MAX_POINT_LIGHT, (uint8_t)Light::s_pointLightsInView.size());
	uint8_t const slCnt = std::min(MAX_SPOT_LIGHT, (uint8_t)Light::s_spotLightsInView.size());
	uint8_t const lightBufSize = (slCnt > 0) ? MAX_POINT_LIGHT + slCnt : plCnt;
	s_lightColorBuf.resize(lightBufSize);
	s_lightPosBuf.resize(lightBufSize);
	s_lightAttnOuterBuf.resize(lightBufSize);
	s_spotDirInnerBuf.resize(slCnt);

	Matrix4x4 const & mtxView = Camera::pCurrent->mtxView;

	for (uint8_t i = 0; i < plCnt; ++i)
	{
		auto & pl = Light::s_pointLightsInView[i];
		ToLinearAccurate(s_lightColorBuf[i], pl.color);
		mtxView.TransformPos(s_lightPosBuf[i], pl.pos);
	}
	s_lightsCnt.z = plCnt;

	for (uint8_t i = 0; i < slCnt; ++i)
	{
		auto & sl = Light::s_spotLightsInView[i];
		uint8_t const bufIdx = MAX_POINT_LIGHT + i;
		ToLinearAccurate(s_lightColorBuf[bufIdx], sl.color);
		mtxView.TransformPos(s_lightPosBuf[bufIdx], sl.pos);
		s_lightAttnOuterBuf[bufIdx] = sl.attnOuter;
		mtxView.TransformPos(s_spotDirInnerBuf[i], sl.dirInner);
		s_spotDirInnerBuf[i].Vec3().Normalize();
	}
	s_lightsCnt.w = slCnt;
}


static void RenderLight()
{
	Light::Submit();

	bgfx::setUniform(uhLightsCnt, &s_lightsCnt);

	uint8_t const plCnt = std::min(MAX_POINT_LIGHT, (uint8_t)Light::s_pointLightsInView.size());
	uint8_t const slCnt = std::min(MAX_SPOT_LIGHT, (uint8_t)Light::s_pointLightsInView.size());
	uint8_t const lightBufSize = (slCnt > 0) ? MAX_POINT_LIGHT + slCnt : plCnt;

	bgfx::setUniform(uhPointColor, s_lightColorBuf.data(), lightBufSize);
	bgfx::setUniform(uhPointPos, s_lightPosBuf.data(), lightBufSize);
	bgfx::setUniform(uhPointAttnOuter, s_lightAttnOuterBuf.data(), lightBufSize);
	bgfx::setUniform(uhSpotDirInner, s_spotDirInnerBuf.data(), slCnt);
}


void ForwardRendering::Render()
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

	RenderLight();

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
