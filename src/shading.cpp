#include "shading.h"
#include "viewState.h"
#include "script.h"
#include "camera.h"
#include "bx/math.h"
#include "drawItem.h"
#include "renderUtil.h"
#include "renderHelper.h"
#include "postProcess.h"
#include "hdr.h"
#include "../../cpp_common/commUtil.h"

#pragma optimize("", off)


namespace Ushuaia
{

#if 0
static bgfx::FrameBufferHandle h_gbufFB;
static bgfx::FrameBufferHandle h_shadeFB;
static bgfx::FrameBufferHandle h_depthFB;
#else
static FrameBuffer *pGBufFB = nullptr;
static FrameBuffer *pDepthFB = nullptr;
static FrameBuffer *pLightFB = nullptr;
static FrameBuffer *pShadeFB = nullptr;
static FrameBuffer *pPostFB = nullptr;
#endif

static bgfx::UniformHandle uhViewVec;
static bgfx::UniformHandle uhRtSize;
static bgfx::UniformHandle uhParam;

static Shader *pDepthTech = nullptr;
static Shader *pAmbLightTech = nullptr;
static Shader *pDirLightTech = nullptr;
static Shader *pPointLightTech = nullptr;
static Shader *pSpotLightTech = nullptr;
static Shader *pShadeTech = nullptr;
static Shader *pCombineTech = nullptr;

static bgfx::UniformHandle uhPointColor;
static bgfx::UniformHandle uhPointPos;	// w for range
static bgfx::UniformHandle uhPointAttnOuter;
static bgfx::UniformHandle uhSpotDirInner;

static std::vector<Color4F> s_lightColorBuf;
static std::vector<Vector4> s_lightPosBuf;
static std::vector<Vector4> s_lightAttnOuterBuf;
static std::vector<Vector4> s_spotDirInnerBuf;

static std::shared_ptr<Mesh> s_pPointLightMesh = nullptr;


void Shading::Init()
{
	uhParam = bgfx::createUniform("uParam", bgfx::UniformType::Vec4);
	uhViewVec = bgfx::createUniform("PV_viewVec", bgfx::UniformType::Vec4);
	uhRtSize = bgfx::createUniform("PV_rtSize", bgfx::UniformType::Vec4);

	pDepthTech = Shader::Load("screen/vs_screen_quad", "screen/fs_linear_depth");
	pDirLightTech = Shader::Load("screen/vs_screen_quad", "lighting/fs_dir_light");
	pPointLightTech = Shader::Load("screen/vs_screen_geo", "lighting/fs_point_light");
	pShadeTech = Shader::Load("screen/vs_screen_quad", "lighting/fs_shading");
	pCombineTech = Shader::Load("screen/vs_screen_quad", "screen/fs_combine");

	bgfx::TextureInfo gbufRTexInfo[3];
	for (int i = 0; i < 3; ++i) {
		gbufRTexInfo[i].cubeMap = false;
		gbufRTexInfo[i].width = g_viewState.width;
		gbufRTexInfo[i].height = g_viewState.height;
		gbufRTexInfo[i].depth = 1;
		gbufRTexInfo[i].numLayers = 1;
		gbufRTexInfo[i].numMips = 1;
	}
	gbufRTexInfo[0].format = bgfx::TextureFormat::RGBA16F;
	gbufRTexInfo[1].format = bgfx::TextureFormat::RGBA16F;
	gbufRTexInfo[2].format = bgfx::TextureFormat::D24S8;
	pGBufFB = new FrameBuffer(gbufRTexInfo, 3);
	pDepthFB = new FrameBuffer(g_viewState.width, g_viewState.height, bgfx::TextureFormat::R32F);
	pLightFB = new FrameBuffer(g_viewState.width, g_viewState.height, bgfx::TextureFormat::RGBA16F);
	pShadeFB = new FrameBuffer(g_viewState.width, g_viewState.height, bgfx::TextureFormat::RGBA16F);
	pPostFB = new FrameBuffer(g_viewState.width, g_viewState.height, bgfx::TextureFormat::RGB10A2);

	uhPointColor = bgfx::createUniform("PV_lightColor", bgfx::UniformType::Vec4);
	uhPointPos = bgfx::createUniform("PV_lightPos", bgfx::UniformType::Vec4);
	uhPointAttnOuter = bgfx::createUniform("PV_lightAttnOuter", bgfx::UniformType::Vec4);
	uhSpotDirInner = bgfx::createUniform("PV_lightSpotDirInner", bgfx::UniformType::Vec4);

	std::vector<Vector3> sphereVtx;
	std::vector<uint16_t> sphereIdx;
	CreateSphere(sphereVtx, sphereIdx, nullptr, 1, 1.f);
	s_pPointLightMesh = Mesh::Create("plSphere", sphereVtx.data(), (uint32_t)sphereVtx.size(),
		PosVertex::s_decl, sphereIdx.data(), (uint32_t)sphereIdx.size());	// exception on exit

	Reset();

	PostProcess::Add(&HDR::Instance());
}


void Shading::Fini()
{
	Lost();

	s_pPointLightMesh = nullptr;

	SafeDelete(pPostFB);
	SafeDelete(pShadeFB);
	SafeDelete(pLightFB);
	SafeDelete(pDepthFB);
	SafeDelete(pGBufFB);

	SafeDestroy(uhViewVec);
	SafeDestroy(uhRtSize);
	SafeDestroy(uhParam);

	SafeDestroy(uhPointColor);
	SafeDestroy(uhPointPos);
	SafeDestroy(uhPointAttnOuter);
	SafeDestroy(uhSpotDirInner);
}


void Shading::Lost()
{

}


void Shading::Reset()
{

}


void Shading::Update()
{
	s_lightColorBuf.clear();
	s_lightPosBuf.clear();
	s_lightAttnOuterBuf.clear();
	s_spotDirInnerBuf.clear();

	size_t const plCnt = Light::s_visiblePointLights.size();
	size_t const slCnt = Light::s_visibleSpotLights.size();
	s_lightColorBuf.resize(plCnt + slCnt);
	s_lightPosBuf.resize(plCnt + slCnt);
	s_lightAttnOuterBuf.resize(plCnt + slCnt);
	s_spotDirInnerBuf.resize(slCnt);

	Matrix4x4 const & mtxView = Camera::pCurrent->mtxView;

	for (size_t i = 0; i < plCnt; ++i) {
		auto & pl = Light::s_visiblePointLights[i];
		ToLinearAccurate(s_lightColorBuf[i], pl.color);
		mtxView.TransformPos(s_lightPosBuf[i], pl.pos);
	}

	for (size_t i = 0; i < slCnt; ++i) {
		auto & sl = Light::s_visibleSpotLights[i];
		size_t const bufIdx = plCnt + i;
		ToLinearAccurate(s_lightColorBuf[bufIdx], sl.color);
		mtxView.TransformPos(s_lightPosBuf[bufIdx], sl.pos);
		s_lightAttnOuterBuf[i] = sl.attnOuter;
		mtxView.TransformPos(s_spotDirInnerBuf[i], sl.dirInner);
		s_spotDirInnerBuf[i].Vec3().Normalize();
	}
}


static inline void UpdateScreenParams()
{
	float w = FrameBuffer::CurrFB()->Width();
	float h = FrameBuffer::CurrFB()->Height();
	Vector4 vec4Param {
		w, h,
		ViewState::texelOffset / w,
		ViewState::texelOffset / h
	};
	bgfx::setUniform(uhRtSize, &vec4Param);
}

static inline void UpdateViewParams()
{
	Vector3 vVec = Camera::pCurrent->ViewExpansionVector();
	Vector4 vec4Param { vVec.x, vVec.y, vVec.z, 0.f };
	bgfx::setUniform(uhViewVec, &vec4Param);
}


static void LinearizeDepth()
{
	bgfx::setMarker("Linear Depth");
	pDepthFB->Setup(nullptr, bgfx::ViewMode::Sequential, true);

	auto pCam = Camera::pCurrent;

	float q = pCam->far / (pCam->far - pCam->near);
	Vector4 camParam { pCam->near * q, q, pCam->far, 1.f / pCam->far };

	bgfx::setUniform(uhParam, camParam.v);
	uint32_t pointSampler = BGFX_SAMPLER_UVW_CLAMP | BGFX_SAMPLER_MIP_POINT
		| BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT;
	SetTexture(0, "s_tex0", pGBufFB->pTex(2), pointSampler);
	bgfx::setState(BGFX_STATE_WRITE_RGB);
	PostProcess::DrawFullScreen(pDepthTech);
}


static void RenderLight()
{
	bgfx::setMarker("Lighting");
	auto pCam = Camera::pCurrent;
	Matrix4x4 mtxLight;

	uint64_t st_lightAdd = 
		BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_ONE)
		| BGFX_STATE_BLEND_EQUATION(BGFX_STATE_BLEND_EQUATION_ADD)
		| BGFX_STATE_WRITE_RGB;

	pLightFB->Setup(pCam, bgfx::ViewMode::Sequential, true);
	
	Light::Submit();

	uint32_t pointSampler = BGFX_SAMPLER_UVW_CLAMP | BGFX_SAMPLER_MIP_POINT
		| BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT;

	// Directional
	UpdateViewParams();
	SetTexture(0, "s_tex0", pGBufFB->pTex(0), pointSampler);
	bgfx::setState(BGFX_STATE_WRITE_RGB);
	PostProcess::DrawFullScreen(pDirLightTech);

	size_t const plCnt = Light::s_visiblePointLights.size();
	size_t const slCnt = Light::s_visibleSpotLights.size();

	// Point
	auto const & plPG = s_pPointLightMesh->groups[0];
	for (size_t i = 0; i < plCnt; ++i) {
		auto const & litPos = Light::s_visiblePointLights[i].pos;

		mtxLight.SetSRT({ litPos.w, litPos.w, litPos.w },
			{ 0,0,0 }, {litPos.x, litPos.y, litPos.z});
		bgfx::setTransform(mtxLight.v);

		Color4F lc = s_lightColorBuf[i];
		Vector4 lp = s_lightPosBuf[i];
		bgfx::setUniform(uhPointColor, &s_lightColorBuf[i]);
		bgfx::setUniform(uhPointPos, &s_lightPosBuf[i]);
		//bgfx::setUniform(uhPointAttnOuter, &s_lightAttnOuterBuf[i]);
		//bgfx::setUniform(uhSpotDirInner, &s_spotDirInnerBuf[i]);

		SetTexture(0, "s_tex0", pGBufFB->pTex(0), pointSampler);
		SetTexture(1, "s_tex1", pDepthFB->pTex(), pointSampler);
		
		bgfx::setState(st_lightAdd);
		//bgfx::setState(BGFX_STATE_WRITE_RGB);
		UpdateScreenParams();
		UpdateViewParams();

		bgfx::setIndexBuffer(plPG.hIB);
		bgfx::setVertexBuffer(0, plPG.hVB);

		bgfx::submit(FrameBuffer::ViewID(), pPointLightTech->Tech());
	}
}


void Shading::Render()
{
	auto pCam = Camera::pCurrent;

	bgfx::setMarker("GeoBuffer");
	pGBufFB->Setup(pCam, bgfx::ViewMode::Default, true);

	uint64_t overrideSt0 = BGFX_STATE_MASK;
	uint64_t overrideSt1 = 0
		| BGFX_STATE_WRITE_RGB
		| BGFX_STATE_WRITE_A
		| BGFX_STATE_WRITE_Z
		| BGFX_STATE_DEPTH_TEST_LESS
		| BGFX_STATE_MSAA
	;
	uint32_t const pointSampler = BGFX_SAMPLER_UVW_CLAMP | BGFX_SAMPLER_MIP_POINT
		| BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT;

	DrawChannel::Gather();

	DrawChannel::DrawOpaque(overrideSt0, overrideSt1);

	DrawChannel::ClearAll();

	LinearizeDepth();

	RenderLight();

	bgfx::setMarker("Shading");
	pShadeFB->Setup(nullptr, bgfx::ViewMode::Sequential, true);
	UpdateViewParams();
	SetTexture(0, "s_tex0", pGBufFB->pTex(0), pointSampler);
	SetTexture(1, "s_tex1", pGBufFB->pTex(1), pointSampler);
	SetTexture(2, "s_tex2", pLightFB->pTex(), pointSampler);
	bgfx::setState(BGFX_STATE_WRITE_RGB);
	PostProcess::DrawFullScreen(pShadeTech);

	PostProcess::Render(pShadeFB->pTex(), pPostFB);

	bgfx::setMarker("Final Combine");
	uint32_t const linearSampler = BGFX_SAMPLER_UVW_CLAMP | BGFX_SAMPLER_MIP_POINT;
	FrameBuffer::BackBuf()->Setup(nullptr, bgfx::ViewMode::Sequential, false);
	Vector4 const texTile { 0.2f, 0.1f, 0.8f, 0.8f };
	bgfx::setUniform(uhParam, texTile.v);
	SetTexture(0, "s_tex0", pPostFB->pTex(), linearSampler);
	SetTexture(1, "s_tex1", pDepthFB->pTex(), linearSampler);
	DrawScreenQuad(pCombineTech,
		BGFX_STATE_WRITE_RGB , texTile.x,texTile.y, texTile.z,texTile.w);
}

}
