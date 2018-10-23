#include "shading.h"
#include "viewState.h"
#include "script.h"
#include "camera.h"
#include "bx/math.h"
#include "drawItem.h"
#include "renderUtil.h"
#include "postProcess.h"
#include "frameBuffer.h"
#include "../../cpp_common/commUtil.h"

#pragma optimize("", off)


namespace Ushuaia
{

decltype(Shading::hDepthTex) Shading::hDepthTex = BGFX_INVALID_HANDLE;
#if 0
static bgfx::FrameBufferHandle h_gbufFB;
static bgfx::FrameBufferHandle h_shadeFB;
static bgfx::FrameBufferHandle h_depthFB;
#else
static FrameBuffer *pFbGBuf = nullptr;
static FrameBuffer *pFbDepth = nullptr;
static FrameBuffer *pFbLight = nullptr;
static FrameBuffer *pFbShade = nullptr;
#endif

static bgfx::UniformHandle uhViewVec;
static bgfx::UniformHandle uhRtSize;
static bgfx::UniformHandle s_Sampler[3];
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
	std::string const smpl = "s_tex";
	for (uint8_t i = 0; i < ArrayCount(s_Sampler); ++i) {
		s_Sampler[i] = bgfx::createUniform((smpl+std::to_string(i)).c_str()
			, bgfx::UniformType::Int1);
	}
	uhParam = bgfx::createUniform("uParam", bgfx::UniformType::Vec4);
	uhViewVec = bgfx::createUniform("PV_viewVec", bgfx::UniformType::Vec4);
	uhRtSize = bgfx::createUniform("PV_rtSize", bgfx::UniformType::Vec4);

	pDepthTech = Shader::Load("vs_screen_quad", "fs_linear_depth");
	pDirLightTech = Shader::Load("vs_screen_quad", "fs_dir_light");
	pPointLightTech = Shader::Load("vs_screen_geo", "fs_point_light");
	pShadeTech = Shader::Load("vs_screen_quad", "fs_shading");
	pCombineTech = Shader::Load("vs_screen_quad", "fs_combine");

	bgfx::TextureFormat::Enum gbufFmt[3] = {
		bgfx::TextureFormat::RGBA16F,
		bgfx::TextureFormat::RGBA16F,
		bgfx::TextureFormat::D24S8
	};
	pFbGBuf = new FrameBuffer(g_viewState.width, g_viewState.height, 3, gbufFmt);
	pFbDepth = new FrameBuffer(g_viewState.width, g_viewState.height, bgfx::TextureFormat::R32F);
	pFbLight = new FrameBuffer(g_viewState.width, g_viewState.height, bgfx::TextureFormat::RGBA16F);
	pFbShade = new FrameBuffer(g_viewState.width, g_viewState.height, bgfx::TextureFormat::RGBA16F);

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
}


void Shading::Fini()
{
	Lost();

	s_pPointLightMesh = nullptr;

	SafeDelete(pFbShade);
	SafeDelete(pFbLight);
	SafeDelete(pFbDepth);
	SafeDelete(pFbGBuf);

	bgfx::destroy(uhViewVec);
	bgfx::destroy(uhRtSize);
	for (uint8_t i = 0; i < ArrayCount(s_Sampler); ++i) {
		if (isValid(s_Sampler[i]))
			bgfx::destroy(s_Sampler[i]);
	}
	bgfx::destroy(uhParam);

	bgfx::destroy(uhPointColor);
	bgfx::destroy(uhPointPos);
	bgfx::destroy(uhPointAttnOuter);
	bgfx::destroy(uhSpotDirInner);
}


void Shading::Lost()
{
	pFbShade->Lost();
	pFbLight->Lost();
	pFbDepth->Lost();
	pFbGBuf->Lost();
}


void Shading::Reset()
{
#if 0
	uint64_t const samplerFlags = 0
		| BGFX_TEXTURE_RT
		| BGFX_SAMPLER_MIN_POINT
		| BGFX_SAMPLER_MAG_POINT
		| BGFX_SAMPLER_MIP_POINT
		| BGFX_SAMPLER_U_CLAMP
		| BGFX_SAMPLER_V_CLAMP
		;

	bgfx::TextureHandle gbufRT[3] = {
		bgfx::createTexture2D(g_viewState.width, g_viewState.height,
			false, 1, bgfx::TextureFormat::RGBA16F, samplerFlags),
		bgfx::createTexture2D(g_viewState.width, g_viewState.height,
			false, 1, bgfx::TextureFormat::RGBA16F, samplerFlags),
		bgfx::createTexture2D(g_viewState.width, g_viewState.height,
			false, 1, bgfx::TextureFormat::D24S8, samplerFlags),
	};
	h_gbufFB = bgfx::createFrameBuffer(ArrayCount(gbufRT), gbufRT, true);

	hDepthTex = bgfx::createTexture2D(g_viewState.width, g_viewState.height,
		false, 1, bgfx::TextureFormat::R32F, samplerFlags);
	h_depthFB = bgfx::createFrameBuffer(1, &hDepthTex, true);

	h_shadeFB = bgfx::createFrameBuffer(g_viewState.width, g_viewState.height
		, bgfx::TextureFormat::RGBA16F, samplerFlags);
#else
	pFbGBuf->Reset();
	pFbDepth->Reset();
	pFbLight->Reset();
	pFbShade->Reset();

	hDepthTex = pFbDepth->TexHandle(0);
#endif
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
	pFbDepth->Setup(nullptr, bgfx::ViewMode::Sequential, true);

	auto pCam = Camera::pCurrent;

	float q = pCam->far / (pCam->far - pCam->near);
	Vector4 camParam { pCam->near * q, q, pCam->far, 1.f / pCam->far };

	bgfx::setUniform(uhParam, camParam.v);
	bgfx::setTexture(0, s_Sampler[0], pFbGBuf->TexHandle(2));
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

	pFbLight->Setup(pCam, bgfx::ViewMode::Sequential, true);
	
	Light::Submit();

	// Directional
	UpdateViewParams();
	bgfx::setTexture( 0, s_Sampler[0], pFbGBuf->TexHandle(0) );
	bgfx::setTexture( 1, s_Sampler[1], pFbDepth->TexHandle(0) );
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

		bgfx::setTexture( 0, s_Sampler[0], pFbGBuf->TexHandle(0) );
		bgfx::setTexture( 1, s_Sampler[1], pFbDepth->TexHandle(0) );
		
		bgfx::setState(st_lightAdd);
		//bgfx::setState(BGFX_STATE_WRITE_RGB);
		UpdateScreenParams();
		UpdateViewParams();

		bgfx::setIndexBuffer(plPG.hIB);
		bgfx::setVertexBuffer(0, plPG.hVB);

		bgfx::submit(FrameBuffer::CurrFB()->ViewID(), pPointLightTech->Tech());
	}
}


void Shading::Render()
{
	auto pCam = Camera::pCurrent;

	bgfx::setMarker("GeoBuffer");
	pFbGBuf->Setup(pCam, bgfx::ViewMode::Default, true);

	uint64_t overrideSt0 = RenderState::s_val[0];
	uint64_t overrideSt1 = 0
		| BGFX_STATE_WRITE_RGB
		| BGFX_STATE_WRITE_A
		| BGFX_STATE_WRITE_Z
		| BGFX_STATE_DEPTH_TEST_LESS
		| BGFX_STATE_MSAA
	;

	DrawChannel::Gather();

	DrawChannel::DrawOpaque(overrideSt0, overrideSt1);

	DrawChannel::ClearAll();

	LinearizeDepth();

	RenderLight();

	bgfx::setMarker("Shading");
	pFbShade->Setup(nullptr, bgfx::ViewMode::Sequential, true);
	UpdateViewParams();
	bgfx::setTexture( 0, s_Sampler[0], pFbGBuf->TexHandle(0) );
	bgfx::setTexture( 1, s_Sampler[1], pFbDepth->TexHandle(0) );
	bgfx::setTexture( 2, s_Sampler[2], pFbLight->TexHandle(0) );
	bgfx::setState(BGFX_STATE_WRITE_RGB);
	PostProcess::DrawFullScreen(pShadeTech);

	bgfx::setMarker("Final Combine");
	FrameBuffer::BackBuf()->Setup(nullptr, bgfx::ViewMode::Sequential, false);
	Vector4 const texTile { 0.5f, 0.f, 0.5f, 0.5f };
	bgfx::setUniform(uhParam, texTile.v);
	bgfx::setTexture( 0, s_Sampler[0], pFbShade->TexHandle(0) );
	DrawScreenQuad(pCombineTech,
		BGFX_STATE_WRITE_RGB , texTile.x,texTile.y, texTile.z,texTile.w);
}

}
