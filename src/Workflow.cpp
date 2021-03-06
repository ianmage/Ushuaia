#include "Workflow.h"
#include <iostream>
#include <bx/timer.h>
#include "../examples/common/bgfx_utils.h"
#include "../examples/common/entry/entry.h"
#include "ViewState.h"
#include "FileSys.h"
#include "Entity.h"
#include "Camera.h"
#include "VtxDecl.h"
#include "DrawItem.h"
#include "PostProcess.h"
#define DEFERRED_RENDERING	1
#if DEFERRED_RENDERING
#include "Shading.h"
#else
#include "ForwardRendering.h"
#endif
#include "Script.h"
#include "../../cpp_common/StrUtil.h"
#include "AppConf.h"

//#pragma optimize("", off)
#define TEST	0


namespace Ushuaia
{
#if TEST
static Mesh mesh;
static bgfx::ProgramHandle hTech = BGFX_INVALID_HANDLE;
#endif

bool Init()
{
	//entry::setCurrentDir("/Users/iAn/Programs/bgfx/examples/runtime/");
	//bool fExist = bx::open(entry::getFileReader(), "res/shaders/glsl/vs_mesh.bin");
	//auto curDir = entry::getCurrentDir();
#ifdef _WIN32
#elif defined(__APPLE__) && defined(__MACH__)
#endif
	//auto curDir = fs::current_path().c_str();
	//DBG("curDir : %s", curDir);
	if (StrEndsWith(fs::current_path().string().c_str(), "bin"))
		fs::current_path("../res");
	else
		fs::current_path("../../res");
#if TEST
	hTech = loadProgram("vs_exam", "fs_exam");

	mesh.Load("meshes/bunny.bin");
#else
	InitVtxDecl();

	Camera::InitDefault();

	float const camFovY = 60.f;
	float const camAspect = float(g_viewState.width) / float(g_viewState.height);
	float const camNear = 0.1f;
	float const camFar = 2000.f;
	//float const projHeight = bx::tan(bx::toRad(camFovY) * 0.5f);
	//float const projWidth = projHeight * camAspect;
	Camera::pCurrent->Init(camFovY, camAspect, camNear, camFar);

	Vector3 const initCamPos{ 0.f, 60.f, -105.f };
	Camera::pCurrent->SetPos(initCamPos);
	Camera::pCurrent->SetVerticalAngle(-0.45f);

	ViewState::texelOffset = bgfx::getRendererType() == bgfx::RendererType::Direct3D9
		? 0.5f : 0.f;

	Shader::Init();
	Light::Init();

	FrameBuffer::Init();
	DrawChannel::Init();
#if DEFERRED_RENDERING
	Shading::Init();
#else
	ForwardRendering::Init();
#endif

	InitScriptData();
#endif

	return true;
}


bool Fini()
{
#if TEST
	mesh.unLoad();

	bgfx::destroy(hTech);
#else
	FiniScriptData();
#endif

	Entity::Fini();
	Model::Fini();
	//Material::ClearAll();

#if DEFERRED_RENDERING
	Shading::Fini();
#else
	ForwardRendering::Fini();
#endif
	PostProcessor::FiniAll();
	Light::Fini();
	FrameBuffer::Fini();
	Shader::ClearAll();

	SamplerMgr::Fini();
	TexMgr::Fini();

	Camera::FiniDefault();

	return true;
}


bool Update()
{
	// Timer
	int64_t now = bx::getHPCounter();
	static int64_t last = now;
	int64_t frameTime = now - last;
	last = now;
	double const freq = double(bx::getHPFrequency());
	AppConf::deltaTime = static_cast<float>(frameTime / freq);
#if TEST
	bgfx::setViewRect(0, 0, 0, bgfx::BackbufferRatio::Equal);
	// This dummy draw call is here to make sure that view 0 is cleared
	// if no other draw calls are submitted to view 0.
	//bgfx::touch(0);
	bgfx::setViewFrameBuffer(0, BGFX_INVALID_HANDLE);
	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH
			, 0x30303000, 1.f, 0);

	float vAt[3]  = { 0.0f, 1.0f,  0.0f };
	float vEye[3] = { 0.0f, 1.0f, -2.5f };
	float mtxView[16];

	bx::mtxLookAt(mtxView, vEye, vAt);

	float mtxProj[16];
	float aspect = float(g_viewState.width) / float(g_viewState.height);
	bx::mtxProj(mtxProj, 60.f, aspect, 0.1f, 100.f, bgfx::getCaps()->homogeneousDepth);
	bgfx::setViewTransform(0, mtxView, mtxProj);

	float mtxModel[16];
	bx::mtxRotateXY(mtxModel, 0.f, 0.f);

	uint64_t _state = 0
		| BGFX_STATE_WRITE_RGB
		| BGFX_STATE_WRITE_A
		| BGFX_STATE_WRITE_Z
		| BGFX_STATE_DEPTH_TEST_LESS
		| BGFX_STATE_CULL_CCW
		| BGFX_STATE_MSAA
		;
	bgfx::setTransform(mtxModel);
	bgfx::setState(_state);
	mesh.submit(0, hTech);
#else
	Camera::pCurrent->Update();

	Light::UpdateAll(Camera::pCurrent);

#if DEFERRED_RENDERING
	Shading::Update();

	Shading::Render();
#else
	ForwardRendering::Update();
	ForwardRendering::Render();
#endif
#endif

	UpdateScript();

	// Use debug font to print information about this example.
	bgfx::dbgTextClear();
	bgfx::dbgTextPrintf(0, 1, 0x0f, "Color can be changed with ANSI \x1b[9;me\x1b[10;ms\x1b[11;mc\x1b[12;ma\x1b[13;mp\x1b[14;me\x1b[0m code too.");

	bgfx::Stats const * stats = bgfx::getStats();
	bgfx::dbgTextPrintf(0, 2, 0x0f, "Backbuffer %dW x %dH in pixels, debug text %dW x %dH in characters."
		, stats->width, stats->height
		, stats->textWidth, stats->textHeight
	);

	FrameBuffer::Update();

	return true;
}

}
