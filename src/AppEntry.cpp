/*
 * Copyright 2011-2018 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

#include "ViewState.h"
#include "AppConf.h"
#include "../examples/common/bgfx_utils.h"
#include "../examples/common/imgui/imgui.h"
#include "Workflow.h"


class MyApp : public entry::AppI
{
public:
	MyApp(char const * _name, char const * _description)
		: entry::AppI(_name, _description)
	{
	}


	void init(int32_t _argc, char const * const * _argv, uint32_t _width, uint32_t _height) override
	{
		Args args(_argc, _argv);

		Ushuaia::g_viewState.width  = static_cast<uint16_t>(_width);
		Ushuaia::g_viewState.height = static_cast<uint16_t>(_height);
	#ifdef _DEBUG
		m_debug = BGFX_DEBUG_TEXT;
	#else
		m_debug = BGFX_DEBUG_NONE;
	#endif
		m_reset = BGFX_RESET_VSYNC;

		bgfx::Init init;
		init.type     = args.m_type;
		init.vendorId = args.m_pciId;
		init.resolution.width  = _width;
		init.resolution.height = _height;
		init.resolution.reset  = m_reset;
	#ifdef _DEBUG
		init.debug = false;
	#else
		init.debug = false;
	#endif
		init.profile = false;
		bgfx::init(init);

		// Enable debug text.
		bgfx::setDebug(m_debug);

		// Set view 0 clear state.
		//bgfx::setViewClear(0
		//	, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH
		//	, 0x303030ff
		//	, 1.0f
		//	, 0
		//);

		imguiCreate();

		Ushuaia::Init();
	}


	virtual int shutdown() override
	{
		Ushuaia::Fini();

		imguiDestroy();

		// Shutdown bgfx.
		bgfx::shutdown();

		return 0;
	}


	bool update() override
	{
		auto & mouseSt = Ushuaia::AppConf::mouseState;
		uint32_t width = Ushuaia::g_viewState.width;
		uint32_t height = Ushuaia::g_viewState.height;
		if (!entry::processEvents(width, height, m_debug, m_reset, &mouseSt) )
		{
			Ushuaia::g_viewState.width = static_cast<uint16_t>(width);
			Ushuaia::g_viewState.height = static_cast<uint16_t>(height);

			imguiBeginFrame(mouseSt.m_mx,  mouseSt.m_my
				, (mouseSt.m_buttons[entry::MouseButton::Left  ] ? IMGUI_MBUT_LEFT   : 0)
				| (mouseSt.m_buttons[entry::MouseButton::Right ] ? IMGUI_MBUT_RIGHT  : 0)
				| (mouseSt.m_buttons[entry::MouseButton::Middle] ? IMGUI_MBUT_MIDDLE : 0)
				,  mouseSt.m_mz
				, Ushuaia::g_viewState.width, Ushuaia::g_viewState.height
			);

			showExampleDialog(this);

			imguiEndFrame();

			// Set view 0 default viewport.
			//bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));

			// This dummy draw call is here to make sure that view 0 is cleared
			// if no other draw calls are submitted to view 0.
			bgfx::touch(0);

			Ushuaia::Update();

			return true;
		}

		return false;
	}

	uint32_t m_debug;
	uint32_t m_reset;
};


ENTRY_IMPLEMENT_MAIN(MyApp, "myApp", "Initialization and debug text.");
