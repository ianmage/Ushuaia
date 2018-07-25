#include "shading.h"
#include "viewState.h"
#include "resData.h"
#include "camera.h"
#include "bx/math.h"
#include "drawItem.h"
#include "renderUtil.h"

//#pragma optimize("", off)


namespace Ushuaia
{

static bgfx::ViewId const RENDER_PASS_GEOMETRY_ID = 0;
static bgfx::ViewId const RENDER_PASS_SHADING_ID = 1;
static bgfx::ViewId const RENDER_PASS_COMBINE_ID = 2;

static bgfx::FrameBufferHandle h_gbufFB;
static bgfx::FrameBufferHandle h_shadeFB;

static bgfx::UniformHandle s_albedoSampler, s_normalSampler;

static Shader* pCombineTech;
static std::shared_ptr<Texture> pTex;

void Shading::Init()
{
	h_gbufFB = BGFX_INVALID_HANDLE;
	h_shadeFB = BGFX_INVALID_HANDLE;

	bgfx::setViewClear(RENDER_PASS_GEOMETRY_ID,
		BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 1.f, 0, 1);
	bgfx::setViewClear(RENDER_PASS_SHADING_ID,
		BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 1.f, 0, 0);

	s_albedoSampler = bgfx::createUniform("S_albedoTex", bgfx::UniformType::Int1);
	s_normalSampler = bgfx::createUniform("S_normalTex", bgfx::UniformType::Int1);

	pCombineTech = Shader::Load("vs_screen_quad", "fs_combine");
	pTex = TexMgr::LoadFromFile("figure-rgba");

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
	if (isValid(s_albedoSampler))
		bgfx::destroy(s_albedoSampler);
	if (isValid(s_normalSampler))
		bgfx::destroy(s_normalSampler);
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
			false, 1, bgfx::TextureFormat::BGRA8, samplerFlags),
		bgfx::createTexture2D(g_viewState.width, g_viewState.height,
			false, 1, bgfx::TextureFormat::BGRA8, samplerFlags),
		bgfx::createTexture2D(g_viewState.width, g_viewState.height,
			false, 1, bgfx::TextureFormat::D24, samplerFlags),
	};
	assert(ArrayCount(gbufRT) == 3);
	h_gbufFB = bgfx::createFrameBuffer(ArrayCount(gbufRT), gbufRT, true);
	h_shadeFB = bgfx::createFrameBuffer(g_viewState.width, g_viewState.height,
		bgfx::TextureFormat::BGRA8, samplerFlags);
	assert(isValid(h_gbufFB));
	bgfx::setViewRect(RENDER_PASS_GEOMETRY_ID, 0, 0, g_viewState.width, g_viewState.height);
	bgfx::setViewRect(RENDER_PASS_SHADING_ID, 0, 0, g_viewState.width, g_viewState.height);
	bgfx::setViewRect(RENDER_PASS_COMBINE_ID, 0, 0, g_viewState.width, g_viewState.height);

	bgfx::setViewFrameBuffer(RENDER_PASS_GEOMETRY_ID, h_gbufFB);
	bgfx::setViewFrameBuffer(RENDER_PASS_SHADING_ID, h_shadeFB);
	bgfx::setViewFrameBuffer(RENDER_PASS_COMBINE_ID, BGFX_INVALID_HANDLE);

	bgfx::Caps const * caps = bgfx::getCaps();
	Matrix4x4 mtxOrtho;
	bx::mtxOrtho(mtxOrtho.v, 0.f, 1.f, 1.f, 0.f, 0.f, 100.f, 0.f, caps->homogeneousDepth);
	bgfx::setViewTransform(RENDER_PASS_SHADING_ID, nullptr, mtxOrtho.v);
	bgfx::setViewTransform(RENDER_PASS_COMBINE_ID, nullptr, mtxOrtho.v);
}


void Shading::Render()
{
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

	assert(isValid(h_gbufFB));
	assert(isValid(bgfx::getTexture(h_gbufFB, 0)));
	assert(isValid(bgfx::getTexture(h_gbufFB, 1)));
	assert(isValid(bgfx::getTexture(h_gbufFB, 2)));

	bgfx::setTexture( 0, s_albedoSampler, bgfx::getTexture(h_gbufFB, 0) );
	bgfx::setTexture( 1, s_normalSampler, bgfx::getTexture(h_gbufFB, 1) );
	bgfx::setState(BGFX_STATE_WRITE_RGB);
	ScreenSpaceQuad(g_viewState.width, g_viewState.height);
	bgfx::submit(RENDER_PASS_COMBINE_ID, pCombineTech->Tech());
}

}
