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

//#pragma optimize("", off)


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

static bgfx::UniformHandle s_Sampler[3];
static bgfx::UniformHandle uParam;

static Shader *pDepthTech = nullptr;
static Shader *pPointLightTech = nullptr;
static Shader *pSpotLightTech = nullptr;
static Shader *pDirLightTech = nullptr;
static Shader *pAmbLightTech = nullptr;
static Shader *pShadeTech = nullptr;
static Shader *pCombineTech = nullptr;


void Shading::Init()
{
	std::string const smpl = "s_tex";
	for (uint8_t i = 0; i < ArrayCount(s_Sampler); ++i) {
		s_Sampler[i] = bgfx::createUniform((smpl+std::to_string(i)).c_str()
			, bgfx::UniformType::Int1);
	}
	uParam = bgfx::createUniform("uParam", bgfx::UniformType::Vec4);

	pDepthTech = Shader::Load("vs_screen_quad", "fs_linear_depth");
	pDirLightTech = Shader::Load("vs_screen_quad", "fs_dir_light");
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

	Reset();
}


void Shading::Fini()
{
	Lost();

	SafeDelete(pFbShade);
	SafeDelete(pFbLight);
	SafeDelete(pFbDepth);
	SafeDelete(pFbGBuf);

	for (uint8_t i = 0; i < ArrayCount(s_Sampler); ++i) {
		if (isValid(s_Sampler[i]))
			bgfx::destroy(s_Sampler[i]);
	}
	if (isValid(uParam))
		bgfx::destroy(uParam);
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
#if 1	// may this part move to update (per-frame) ?
	bgfx::setViewRect(RENDER_PASS::GEOMETRY_ID, 0, 0, g_viewState.width, g_viewState.height);
	bgfx::setViewFrameBuffer(RENDER_PASS::GEOMETRY_ID, pFbGBuf->Handle());
#endif

	bgfx::setViewClear(RENDER_PASS::GEOMETRY_ID,
		BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 1.f, 0, 1);
}


void Shading::Update()
{
	PostProcess::Update();
}


static void RenderLight()
{
	Light::Submit();

	PostProcess::NewFrameBuf(pFbLight, true);

	// Directional
	bgfx::setTexture( 0, s_Sampler[0], pFbGBuf->TexHandle(0) );
	bgfx::setTexture( 1, s_Sampler[1], pFbDepth->TexHandle(0) );
	bgfx::setState(BGFX_STATE_WRITE_RGB);
	PostProcess::DrawFullScreen(pDirLightTech);
	
	// Point
}


void Shading::Render()
{
	bgfx::setViewFrameBuffer(RENDER_PASS::GEOMETRY_ID, pFbGBuf->Handle());

	auto pCam = Camera::pCurrent;
	bgfx::setViewTransform(RENDER_PASS::GEOMETRY_ID, pCam->mtxView.v, pCam->mtxProj.v);

	Light::Submit();

	uint64_t overrideSt0 = RenderState::s_val[0];
	uint64_t overrideSt1 = 0
		| BGFX_STATE_WRITE_RGB
		| BGFX_STATE_WRITE_A
		| BGFX_STATE_WRITE_Z
		| BGFX_STATE_DEPTH_TEST_LESS
		| BGFX_STATE_MSAA
	;

	DrawChannel::Gather();

	DrawChannel::DrawOpaque(RENDER_PASS::GEOMETRY_ID, overrideSt0, overrideSt1);

	DrawChannel::ClearAll();

	PostProcess::NewFrameBuf(pFbDepth, true);
	bgfx::setTexture( 0, s_Sampler[0], pFbGBuf->TexHandle(2) );
	float q = pCam->far / (pCam->far - pCam->near);
	Vector4 v4Param{pCam->near, pCam->far, q, pCam->near * q};
	bgfx::setUniform(uParam, v4Param.v);
	bgfx::setState(BGFX_STATE_WRITE_RGB);
	PostProcess::DrawFullScreen(pDepthTech);

	RenderLight();

	PostProcess::NewFrameBuf(pFbShade, true);
	bgfx::setTexture( 0, s_Sampler[0], pFbGBuf->TexHandle(0) );
	bgfx::setTexture( 1, s_Sampler[1], pFbDepth->TexHandle(0) );
	bgfx::setTexture( 2, s_Sampler[2], pFbLight->TexHandle(0) );
	bgfx::setState(BGFX_STATE_WRITE_RGB);
	PostProcess::DrawFullScreen(pShadeTech);

	PostProcess::NewFrameBuf(nullptr, false);
	bgfx::setTexture( 0, s_Sampler[0], pFbShade->TexHandle(0) );
	DrawScreenQuad(PostProcess::ViewID(), pCombineTech,
		BGFX_STATE_WRITE_RGB , 0.f, 0.f, 1.f, 1.f);
}

}
