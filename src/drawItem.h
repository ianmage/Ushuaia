#pragma once

#include "model.h"


namespace Ushuaia
{

	struct DrawItem
	{
		Model* pModel;
		Matrix4x4 mtx;
	};

	struct InstanceItem
	{
		Model* pModel;
		std::vector<Matrix4x4> mtxTransform;
	};

	struct DrawChannel
	{
	public:
		static bool s_supportInstancing;

		static void init();
		static void gather();
		static void drawOpaque(bgfx::ViewId viewId, uint64_t override0, uint64_t override1);
		static void clearAll();

	private:
		static std::vector<DrawItem> s_opaque;
		static std::vector<InstanceItem> s_instance;
	};

}
