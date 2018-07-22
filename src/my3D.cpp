#include "my3D.h"
#include "../examples/common/bgfx_utils.h"
#include "../examples/common/entry/entry.h"
#include "viewState.h"
#include <boost/filesystem.hpp>
#include <iostream>
#include "entity.h"
#include "camera.h"
#include "vtxDecl.h"
#include "drawItem.h"
#define DEFERRED_RENDERING	0
#if DEFERRED_RENDERING
#include "shading.h"
#else
#include "forwardRendering.h"
#endif
#include "resData.h"

//#pragma optimize("", off)
#define TEST	0

namespace fs = boost::filesystem;


namespace Ushuaia
{
#if TEST
static Mesh mesh;
static bgfx::ProgramHandle hProgram = BGFX_INVALID_HANDLE;
#endif

bool Init()
{
	//entry::setCurrentDir("/Users/iAn/Programs/bgfx/examples/runtime/");
	//bool fExist = bx::open(entry::getFileReader(), "res/shaders/glsl/vs_mesh.bin");
	//auto curDir = entry::getCurrentDir();
#ifdef _WIN32
	fs::current_path("../res");
#elif defined(__APPLE__) && defined(__MACH__)
	fs::current_path("../res");
#endif
	//auto curDir = fs::current_path().c_str();
	//DBG("curDir : %s", curDir);
#if TEST
	hProgram = loadProgram("vs_exam", "fs_exam");

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

	Shader::Init();
	Light::Init();

	DrawChannel::Init();
#if DEFERRED_RENDERING
	Shading::Init();
#else
	ForwardRendering::Init();
#endif

	InitResData();
#endif

	return true;
}


bool Fini()
{
#if TEST
	mesh.unLoad();

	bgfx::destroy(hProgram);
#else
	FiniResData();
#endif

	Entity::Fini();
	Model::Fini();
	//Material::ClearAll();

#if DEFERRED_RENDERING
	Shading::Fini();
#else
	ForwardRendering::Fini();
#endif
	Light::Fini();
	Shader::ClearAll();

	Camera::FiniDefault();

	return true;
}


bool Update()
{
#if TEST
	bgfx::setViewRect(0, 0, 0,
		uint16_t(g_viewState.width), uint16_t(g_viewState.height));
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
	mesh.submit(0, hProgram);
#else
	Camera::pCurrent->Update();

	UpdateResData();

	Light::UpdateAll(Camera::pCurrent->mtxView);

#if DEFERRED_RENDERING
	Shading::Update();

	Shading::Render();
#else
	ForwardRendering::Update();
	ForwardRendering::Render();
#endif
#endif

	// Use debug font to print information about this example.
	bgfx::dbgTextClear();
	bgfx::dbgTextPrintf(0, 1, 0x0f, "Color can be changed with ANSI \x1b[9;me\x1b[10;ms\x1b[11;mc\x1b[12;ma\x1b[13;mp\x1b[14;me\x1b[0m code too.");

	bgfx::Stats const * stats = bgfx::getStats();
	bgfx::dbgTextPrintf(0, 2, 0x0f, "Backbuffer %dW x %dH in pixels, debug text %dW x %dH in characters."
		, stats->width, stats->height
		, stats->textWidth, stats->textHeight
	);

	return true;
}

}
