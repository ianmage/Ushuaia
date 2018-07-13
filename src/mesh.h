#pragma once

#include <vector>
#include "bx/readerwriter.h"
#include "shader.h"
#include <memory>
#include "math.h"


namespace Ushuaia
{

struct Sphere
{
	Vector3 center;
	float radius;
};


struct Primitive
{
	uint32_t startIndex;
	uint32_t numIndices;
	uint32_t startVertex;
	uint32_t numVertices;

	Sphere sphere;
	Box aabb;
	Matrix4x4 obb;
};


struct Group
{
	Group()
	{
		reset();
	}

	void reset()
	{
		hVB.idx = bgfx::kInvalidHandle;
		hIB.idx = bgfx::kInvalidHandle;
		prims.clear();
	}

	bgfx::VertexBufferHandle hVB;
	bgfx::IndexBufferHandle hIB;
	Sphere sphere;
	Box aabb;
	Matrix4x4 obb;
	std::vector<Primitive> prims;
};


struct Mesh
{
	bgfx::VertexDecl vtxDecl;

	virtual ~Mesh();

	bool Deserialize();
	void Release();

	std::string const & Name() const { return name_; }

	void Submit(bgfx::ViewId _id, Shader const *_program) const;

	static std::shared_ptr<Mesh> Create(std::string const & _name,
		void const * _vertices, uint32_t _numVertices, bgfx::VertexDecl const & _decl,
		uint16_t const * _indices, uint32_t _numIndices);
	static std::shared_ptr<Mesh> Load(std::string const & _name);

private:
	static std::unordered_map<size_t, std::weak_ptr<Mesh>> s_meshes;
	std::string name_;
	std::vector<Group> groups_;

	Mesh(std::string const & _name);
};

}
