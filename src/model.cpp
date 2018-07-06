#include "model.h"
#include "../../cpp_common/commUtil.h"


namespace Ushuaia
{

	Model::Model(std::string const & _name)
		: name(_name), pMesh(nullptr), pMtl(nullptr)
	{
	}


	void Model::serialize(Writer& writer) const
	{
		writer.String(name);
	}


	std::shared_ptr<Model> Model::create(std::string const & _name)
	{
		size_t id = RT_HASH(_name.c_str());

		auto const & it = s_models.find(id);
		if (it != s_models.end())
		{
			if (!it->second.expired())
				return it->second.lock();
		}

		std::shared_ptr<Model> pModel(new Model(_name));
		s_models.emplace(id, pModel);

		return std::move(pModel);
	}


	void Model::fini()
	{
		s_models.clear();
	}


	void Model::draw(bgfx::ViewId viewId
		, uint64_t override0, uint64_t override1
		, Shader const *pShader)
	{
		if (!pMesh || !pMtl)
			return;

		pMtl->submit(override0, override1);
		pMesh->submit(viewId, pShader ? pShader : pMtl->pShader());
	}

}
