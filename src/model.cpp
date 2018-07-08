#include "model.h"
#include "../../cpp_common/commUtil.h"
#include "serialize.h"


namespace Ushuaia
{

Model::Model(std::string const & _name)
	: name(_name), pMesh(nullptr), pMtl(nullptr)
{
}


void Model::serialize() const
{
	JsonWriter writer;

	if (pMesh) {
		writer.Key("Mesh");
		writer.String(pMesh->name);
	}
	if (pMtl) {
		writer.Key("Material");
		writer.String(pMtl->name);
	}

	writer.save("model/" + name);
}


bool Model::deserialize()
{
	JsonReader reader;
	if (!reader.load("model/" + name))
		return false;

	JsonValue::ConstMemberIterator itr;
	itr = reader.FindMember("Mesh");
	if (itr != reader.MemberEnd()) {
		pMesh = Mesh::load(itr->value.GetString());
	}
	itr = reader.FindMember("Material");
	if (itr != reader.MemberEnd()) {
		pMtl = Material::load(itr->value.GetString());
	}

	return true;
}


std::shared_ptr<Model> Model::load(std::string const & _name)
{
	size_t k = RT_HASH(_name.c_str());

	auto const & it = s_models.find(k);
	if (it != s_models.end()) {
		if (!it->second.expired())
			return it->second.lock();
	}

	std::shared_ptr<Model> pModel(new Model(_name));
	s_models.emplace(k, pModel);

	pModel->deserialize();

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
