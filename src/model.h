#pragma once

#include "mesh.h"
#include "material.h"
#include "renderState.h"
#include <memory>


namespace Ushuaia
{

	struct Model
	{
		Mesh *pMesh;
		Material *pMtl;

		void draw(bgfx::ViewId viewId
			, uint64_t override0 = RenderState::s_val[0], uint64_t override1 = RenderState::s_val[1]
			, Shader const *pShader = nullptr);

		static std::unordered_map<size_t, std::weak_ptr<Model>> s_models;

		static std::shared_ptr<Model> create(char const *_name);

		static void fini();
	};

}
