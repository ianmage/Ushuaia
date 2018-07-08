#pragma once

#include "mesh.h"
#include "material.h"
#include "renderState.h"


namespace Ushuaia
{

struct Model
{
	std::shared_ptr<Mesh> pMesh;
	Material *pMtl;

	std::string name;

	void serialize() const;
	bool deserialize();

	void draw(bgfx::ViewId viewId
		, uint64_t override0 = RenderState::s_val[0], uint64_t override1 = RenderState::s_val[1]
		, Shader const *pShader = nullptr);

	static std::shared_ptr<Model> load(std::string const & _name);

	static void fini();

private:
	Model(std::string const & _name);

	static std::unordered_map<size_t, std::weak_ptr<Model>> s_models;
};

}
