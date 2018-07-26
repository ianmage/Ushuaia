#include "shading.h"
#include "viewState.h"
#include "script.h"
#include "camera.h"
#include "bx/math.h"
#include "drawItem.h"
#include "renderUtil.h"

//#pragma optimize("", off)


namespace Ushuaia
{

decltype(Shading::hDepthTex) Shading::hDepthTex = BGFX_INVALID_HANDLE;

static bgfx::ViewId const RENDER_PASS_GEOMETRY_ID = 0;
static bgfx::ViewId const RENDER_PASS_SHADING_ID = 1;
static bgfx::ViewId const RENDER_PASS_PP_ID = 2;

static bgfx::FrameBufferHandle h_gbufFB;
static bgfx::FrameBufferHandle h_shadeFB;
static bgfx::FrameBufferHandle h_depthFB;

static bgfx::UniformHandle s_Sampler[3];

static Shader *pDepthTech;
static Shader *pCombineTech;


void Shading::Init()
{
	h_gbufFB = BGFX_INVALID_HANDLE;
	h_shadeFB = BGFX_INVALID_HANDLE;

	std::string const smpl = "s_tex";
	for (uint8_t i = 0; i < ArrayCount(s_Sampler); ++i) {
		s_Sampler[i] = bgfx::createUniform((smpl+std::to_string(i)).c_str()
			, bgfx::UniformType::Int1);
	}

	pDepthTech = Shader::Load("vs_screen_quad", "fs_linear_depth");

	pCombineTech = Shader::Load("vs_screen_quad", "fs_combine");

	Reset();
}


void Shading::Fini()
{
	Lost();
}


void Shading::Update()
{
	
}


void Shading::Lost()
{
	if (isValid(h_shadeFB))
		bgfx::destroy(h_shadeFB);
	if (isValid(h_gbufFB))
		bgfx::destroy(h_gbufFB);
	for (uint8_t i = 0; i < ArrayCount(s_Sampler); ++i) {
		if (isValid(s_Sampler[i]))
			bgfx::destroy(s_Sampler[i]);
	}
	if (isValid(h_depthFB))
		bgfx::destroy(h_depthFB);
	if (isValid(hDepthTex))
		bgfx::destroy(hDepthTex);
}


void Shading::Reset()
{
	uint32_t const samplerFlags = 0
		| BGFX_TEXTURE_RT
		| BGFX_TEXTURE_MIN_POINT
		| BGFX_TEXTURE_MAG_POINT
		| BGFX_TEXTURE_MIP_POINT
		| BGFX_TEXTURE_U_CLAMP
		| BGFX_TEXTURE_V_CLAMP
		;

	bgfx::TextureHandle gbufRT[3] = {
		bgfx::createTexture2D(g_viewState.width, g_viewState.height,
			false, 1, bgfx::TextureFormat::RGBA16F, samplerFlags),
		bgfx::createTexture2D(g_viewState.width, g_viewState.height,
			false, 1, bgfx::TextureFormat::RGBA16F, samplerFlags),
		bgfx::createTexture2D(g_viewState.width, g_viewState.height,
			false, 1, bgfx::TextureFormat::D24, samplerFlags),
	};

	h_gbufFB = bgfx::createFrameBuffer(ArrayCount(gbufRT), gbufRT, true);
	h_shadeFB = bgfx::createFrameBuffer(g_viewState.width, g_viewState.height
		, bgfx::TextureFormat::BGRA8, samplerFlags);
	assert(isValid(h_gbufFB));

	h_depthFB = bgfx::createFrameBuffer(g_viewState.width, g_viewState.height
		, bgfx::TextureFormat::R32F, samplerFlags);
	hDepthTex = bgfx::getTexture(h_depthFB);

#if 1	// may this part move to update (per-frame) ?
	bgfx::setViewRect(RENDER_PASS_GEOMETRY_ID, 0, 0, g_viewState.width, g_viewState.height);
	bgfx::setViewRect(RENDER_PASS_SHADING_ID, 0, 0, g_viewState.width, g_viewState.height);
	bgfx::setViewRect(RENDER_PASS_PP_ID, 0, 0, g_viewState.width, g_viewState.height);

	bgfx::setViewFrameBuffer(RENDER_PASS_GEOMETRY_ID, h_gbufFB);
	bgfx::setViewFrameBuffer(RENDER_PASS_SHADING_ID, h_shadeFB);
	bgfx::setViewFrameBuffer(RENDER_PASS_PP_ID, h_depthFB);

	bgfx::Caps const * caps = bgfx::getCaps();
	Matrix4x4 mtxOrtho;
	bx::mtxOrtho(mtxOrtho.v, 0.f, 1.f, 1.f, 0.f, 0.f, 100.f, 0.f, caps->homogeneousDepth);
	bgfx::setViewTransform(RENDER_PASS_SHADING_ID, nullptr, mtxOrtho.v);
	bgfx::setViewTransform(RENDER_PASS_PP_ID, nullptr, mtxOrtho.v);
#endif

	bgfx::setViewClear(RENDER_PASS_GEOMETRY_ID,
		BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 1.f, 0, 1);
	bgfx::setViewClear(RENDER_PASS_SHADING_ID,
		BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 1.f, 0, 0);
	bgfx::setViewClear(RENDER_PASS_PP_ID,
		BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 1.f, 0, 0);
}


void Shading::Render()
{
#if 1
	bgfx::setViewFrameBuffer(RENDER_PASS_GEOMETRY_ID, h_gbufFB);
	bgfx::setViewFrameBuffer(RENDER_PASS_SHADING_ID, h_shadeFB);

	bgfx::setViewMode(RENDER_PASS_PP_ID, bgfx::ViewMode::Sequential);
#endif

	auto pCam = Camera::pCurrent;
	bgfx::setViewTransform(RENDER_PASS_GEOMETRY_ID, pCam->mtxView.v, pCam->mtxProj.v);

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

	DrawChannel::DrawOpaque(RENDER_PASS_GEOMETRY_ID, overrideSt0, overrideSt1);

	DrawChannel::ClearAll();

	bgfx::setViewFrameBuffer(RENDER_PASS_PP_ID, h_depthFB);
	bgfx::setTexture( 0, s_Sampler[0], bgfx::getTexture(h_gbufFB, 2) );
	bgfx::setState(BGFX_STATE_WRITE_RGB);
	ScreenSpaceQuad(g_viewState.width, g_viewState.height);
	bgfx::submit(RENDER_PASS_PP_ID, pDepthTech->Tech());

	bgfx::setViewFrameBuffer(RENDER_PASS_PP_ID, BGFX_INVALID_HANDLE);
	bgfx::setTexture( 0, s_Sampler[0], bgfx::getTexture(h_gbufFB, 0) );
	bgfx::setTexture( 1, s_Sampler[1], bgfx::getTexture(h_gbufFB, 1) );
	bgfx::setTexture( 2, s_Sampler[2], hDepthTex );
	bgfx::setState(BGFX_STATE_WRITE_RGB);
	ScreenSpaceQuad(g_viewState.width, g_viewState.height);
	bgfx::submit(RENDER_PASS_PP_ID, pCombineTech->Tech());
}

}
