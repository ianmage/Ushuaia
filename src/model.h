#pragma once

#include "mesh.h"
#include "material.h"
#include "renderState.h"


namespace Ushuaia
{

struct Model
{
	std::shared_ptr<Mesh> pMesh;
	std::vector<Material> materials;

	virtual ~Model();

	std::string const & Name() const { return name_; }

	void Serialize() const;
	bool Deserialize();

	void Draw(Matrix4x4 const & transform);

	bool SetMtlIndices(std::vector<uint8_t> const & indices);

	static std::shared_ptr<Model> Load(std::string const & _name);
	static void SaveAll();

	static void Fini();

private:
	std::string name_;
	uint32_t mtlIndices_;

	static std::unordered_map<size_t, std::weak_ptr<Model>> s_models;

	Model(std::string const & _name);
};

}
