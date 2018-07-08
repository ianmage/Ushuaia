#include "mesh.h"
#include "../examples/common/entry/entry.h"
#include "ib-compress/indexbufferdecompression.h"
#include "../../cpp_common/commUtil.h"

//#pragma optimize("", off)


namespace bgfx
{
int32_t read(bx::ReaderI* _reader, bgfx::VertexDecl& _decl, bx::Error* _err = NULL);
}


namespace Ushuaia
{

Mesh::~Mesh()
{
	release();
}


std::shared_ptr<Mesh> Mesh::create(void const * _vertices, uint32_t _numVertices, bgfx::VertexDecl const & _decl, uint16_t const * _indices, uint32_t _numIndices)
{
	std::shared_ptr<Mesh> pMesh(new Mesh(""));

	std::vector<uint16_t> idx(_numIndices);
	for (uint32_t i = 0; i < _numIndices; ++i)
	{
		idx[i] =  _indices[i];
	}
	Group group;
	bgfx::Memory const * mem;

	mem = bgfx::alloc(_numVertices * _decl.getStride());
	memcpy(mem->data, _vertices, mem->size);
	group.hVB = bgfx::createVertexBuffer(mem, _decl);

	mem = bgfx::alloc(_numIndices * 2);
	memcpy(mem->data, _indices, mem->size);
	group.hIB = bgfx::createIndexBuffer(mem);

	pMesh->groups.emplace_back(std::move(group));

	return std::move(pMesh);
}


std::shared_ptr<Mesh> Mesh::load(std::string const & _name)
{
	size_t k = RT_HASH(_name.c_str());

	auto const & it = s_meshes.find(k);
	if (it != s_meshes.end()) {
		if (!it->second.expired())
			return it->second.lock();
	}

	std::shared_ptr<Mesh> pMesh(new Mesh(_name));
	s_meshes.emplace(k, pMesh);

	pMesh->deserialize();

	return std::move(pMesh);
}


bool Mesh::deserialize()
{
	std::string filePath = "mesh/" + name + ".bin";

	bx::FileReaderI* pReader = entry::getFileReader();
	if (!bx::open(pReader, filePath.c_str()))
		return false;

#define BGFX_CHUNK_MAGIC_VB  BX_MAKEFOURCC('V', 'B', ' ', 0x1)
#define BGFX_CHUNK_MAGIC_IB  BX_MAKEFOURCC('I', 'B', ' ', 0x0)
#define BGFX_CHUNK_MAGIC_IBC BX_MAKEFOURCC('I', 'B', 'C', 0x0)
#define BGFX_CHUNK_MAGIC_PRI BX_MAKEFOURCC('P', 'R', 'I', 0x0)

	Group group;

	bx::AllocatorI* allocator = entry::getAllocator();

	uint32_t chunk;
	bx::Error err;
	while (4 == bx::read(pReader, chunk, &err)
		&& err.isOk())
	{
		switch (chunk)
		{
		case BGFX_CHUNK_MAGIC_VB:
		{
			bx::read(pReader, group.sphere);
			bx::read(pReader, group.aabb);
			bx::read(pReader, group.obb);

			bgfx::read(pReader, vtxDecl);

			uint16_t stride = vtxDecl.getStride();

			uint16_t numVertices;
			bx::read(pReader, numVertices);
			bgfx::Memory const * mem = bgfx::alloc(numVertices*stride);
			read(pReader, mem->data, mem->size);

			group.hVB = bgfx::createVertexBuffer(mem, vtxDecl);
		}
		break;

		case BGFX_CHUNK_MAGIC_IB:
		{
			uint32_t numIndices;
			bx::read(pReader, numIndices);
			bgfx::Memory const * mem = bgfx::alloc(numIndices * 2);
			bx::read(pReader, mem->data, mem->size);
			group.hIB = bgfx::createIndexBuffer(mem);
		}
		break;

		case BGFX_CHUNK_MAGIC_IBC:
		{
			uint32_t numIndices;
			bx::read(pReader, numIndices);

			bgfx::Memory const * mem = bgfx::alloc(numIndices * 2);

			uint32_t compressedSize;
			bx::read(pReader, compressedSize);

			void* compressedIndices = BX_ALLOC(allocator, compressedSize);

			bx::read(pReader, compressedIndices, compressedSize);

			ReadBitstream rbs((uint8_t const *)compressedIndices, compressedSize);
			DecompressIndexBuffer((uint16_t*)mem->data, numIndices / 3, rbs);

			BX_FREE(allocator, compressedIndices);

			group.hIB = bgfx::createIndexBuffer(mem);
		}
		break;

		case BGFX_CHUNK_MAGIC_PRI:
		{
			uint16_t len;
			bx::read(pReader, len);

			std::string material;
			material.resize(len);
			bx::read(pReader, const_cast<char*>(material.c_str()), len);

			uint16_t num;
			bx::read(pReader, num);

			for (uint32_t ii = 0; ii < num; ++ii)
			{
				bx::read(pReader, len);

				std::string name;
				name.resize(len);
				bx::read(pReader, const_cast<char*>(name.c_str()), len);

				Primitive prim;
				bx::read(pReader, prim.startIndex);
				bx::read(pReader, prim.numIndices);
				bx::read(pReader, prim.startVertex);
				bx::read(pReader, prim.numVertices);
				bx::read(pReader, prim.sphere);
				bx::read(pReader, prim.aabb);
				bx::read(pReader, prim.obb);

				group.prims.push_back(prim);
			}

			groups.push_back(group);
			group.reset();
		}
		break;

		default:
			DBG("%08x at %d", chunk, bx::skip(pReader, 0));
			break;
		}
	}
	bx::close(pReader);
	return true;
}


void Mesh::release()
{
	for (auto const & group : groups)
	{
		bgfx::destroy(group.hVB);

		if (bgfx::isValid(group.hIB))
		{
			bgfx::destroy(group.hIB);
		}
	}
	groups.clear();
}


void Mesh::submit(bgfx::ViewId _id, Shader const *_pProgram) const
{
	for (decltype(groups)::const_iterator it = groups.begin(), itEnd = groups.end(); it != itEnd; ++it)
	{
		Group const & group = *it;

		if (bgfx::isValid(group.hIB))
			bgfx::setIndexBuffer(group.hIB);
		bgfx::setVertexBuffer(0, group.hVB);
		bgfx::submit(_id, _pProgram->hProgram, 0, it != itEnd-1);
	}
}

}
