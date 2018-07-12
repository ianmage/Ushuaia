#include "model.h"
#include "serialize.h"


namespace Ushuaia
{

Model::Model(std::string const & _name)
	: name_(_name), pMesh(nullptr), pMtl(nullptr)
{
}


Model::~Model()
{
	delete pMtl;
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
#if 0
		writer.String(pMtl->Name());
#else
		pMtl->Serialize(writer);
#endif
	}

	writer.Save("model/" + name_);
}


bool Model::Deserialize()
{
	assert(!pMesh && !pMtl);
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
#if 0
		pMtl = Material::Load(itr->value.GetString());
#else
		pMtl = new Material;
		pMtl->Deserialize(itr->value);
#endif
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


void Model::Draw(bgfx::ViewId _viewId
	, uint64_t _overrideSt0, uint64_t _overrideSt1
	, Shader const * _overrideShader)
{
	if (!pMesh || !pMtl)
		return;

	pMtl->Submit(_overrideSt0, _overrideSt1, _overrideShader);
	pMesh->Submit(_viewId, _overrideShader ? _overrideShader : pMtl->GetShader());
}

}
