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

	std::string const & Name() const { return name_; }

	void Serialize() const;
	bool Deserialize();

	void Draw(bgfx::ViewId _viewId
		, uint64_t _overrideSt0 = RenderState::s_val[0]
		, uint64_t _overrideSt1 = RenderState::s_val[1]
		, Shader const * _overrideShader = nullptr);

	static std::shared_ptr<Model> Load(std::string const & _name);

	static void Fini();

private:
	std::string name_;

	static std::unordered_map<size_t, std::weak_ptr<Model>> s_models;

	Model(std::string const & _name);
};

}
