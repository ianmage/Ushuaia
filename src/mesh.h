#pragma once

#include <vector>
#include "bx/readerwriter.h"
#include "shader.h"


namespace My3D
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
		virtual ~Mesh();

		void load(void const * _vertices, uint32_t _numVertices, bgfx::VertexDecl const & _decl, uint16_t const * _indices, uint32_t _numIndices);

		bool load(char const * filePath);

		void unload();

		void submit(bgfx::ViewId _id, Shader const *_pProgram) const;

		bgfx::VertexDecl vtxDecl;
		std::vector<Group> groups;
	};

}
