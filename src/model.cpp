#include "model.h"
#include "../../cpp_common/commUtil.h"
#include "serialize.h"


namespace Ushuaia
{

Model::Model(std::string const & _name)
	: name_(_name), pMesh(nullptr), pMtl(nullptr)
{
}


void Model::Serialize() const
{
	JsonWriter writer;

	if (pMesh) {
		writer.Key("Mesh");
		writer.String(pMesh->Name());
	}
	if (pMtl) {
		writer.Key("Material");
		writer.String(pMtl->Name());
	}

	writer.Save("model/" + name_);
}


bool Model::Deserialize()
{
	JsonReader reader;
	if (!reader.Load("model/" + name_))
		return false;

	JsonValue::ConstMemberIterator itr;
	itr = reader.FindMember("Mesh");
	if (itr != reader.MemberEnd()) {
		pMesh = Mesh::Load(itr->value.GetString());
	}
	itr = reader.FindMember("Material");
	if (itr != reader.MemberEnd()) {
		pMtl = Material::Load(itr->value.GetString());
	}

	return true;
}


std::shared_ptr<Model> Model::Load(std::string const & _name)
{
	size_t k = RT_HASH(_name.c_str());

	decltype(s_models)::const_iterator itr = s_models.find(k);
	if (itr != s_models.end()) {
		if (!itr->second.expired())
			return itr->second.lock();
	}

	std::shared_ptr<Model> pModel(new Model(_name));
	s_models.emplace(k, pModel);

	pModel->Deserialize();

	return std::move(pModel);
}


void Model::Fini()
{
	s_models.clear();
}


void Model::Draw(bgfx::ViewId viewId
	, uint64_t override0, uint64_t override1
	, Shader const *pShader)
{
	if (!pMesh || !pMtl)
		return;

	pMtl->Submit(override0, override1);
	pMesh->Submit(viewId, pShader ? pShader : pMtl->GetShader());
}

}
