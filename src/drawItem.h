#pragma once

#include "model.h"


namespace Ushuaia
{

	struct DrawItem
	{
		Model* pModel;
		Matrix4x4 transform;
	};

	struct InstanceItem
	{
		Model* pModel;
		std::vector<Matrix4x4> transforms;
	};

	struct DrawChannel
	{
	public:
		static bool s_supportInstancing;

		static void Init();
		static void Gather();
		static void DrawOpaque(bgfx::ViewId viewId, uint64_t override0, uint64_t override1);
		static void ClearAll();

	private:
		static std::vector<DrawItem> s_opaque;
		static std::vector<InstanceItem> s_instance;
	};

}
