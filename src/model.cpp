#include "model.h"
#include "serialize.h"
#include "drawItem.h"


namespace Ushuaia
{

Model::Model(std::string const & _name)
	: name_(_name), pMesh(nullptr)
{
}


Model::~Model()
{
}


void Model::Serialize() const
{
	JsonWriter writer;

	if (pMesh) {
		writer.Key("Mesh");
		writer.String(pMesh->Name());
	}

	if (!materials.empty()) {
		writer.Key("Material");
		writer.StartArray();
		for (auto const & m : materials) {
			m.Serialize(writer);
		}
		writer.EndArray();
	}
	if (!mtlIndices.empty()) {
		writer.Key("MtlIndices");
		writer.StartArray();
		for (auto m : mtlIndices) {
			writer.Int(m);
		}
		writer.EndArray();
	}

	writer.Save("model/" + name_);
}


bool Model::Deserialize()
{
	assert(!pMesh && materials.empty());
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
		materials.resize(itr->value.Size());
		uint32_t idx = 0;
		for (auto const & m : itr->value.GetArray()) {
			materials[idx++].Deserialize(m);
		}
	}
	itr = reader.FindMember("MtlIndices");
	if (itr != reader.MemberEnd()) {
		mtlIndices.reserve(itr->value.Size());
		for (auto const & m : itr->value.GetArray()) {
			mtlIndices.push_back(m.GetInt());
		}
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


void Model::SaveAll()
{
	for (auto & m : s_models) {
		if (!m.second.expired())
			m.second.lock()->Serialize();
	}
}


void Model::Fini()
{
	s_models.clear();
}


void Model::Draw(Matrix4x4 const & transform)
{
	if (!pMesh || materials.empty())
		return;

	std::string n = name_;
	size_t gs = pMesh->groups.size();

	assert(mtlIndices.size() == pMesh->groups.size());
	uint8_t idx = 0;
	for (auto const & g : pMesh->groups) {
		Material & mtl = materials[mtlIndices[idx]];
		DrawItem & di = DrawChannel::Add();
		di.primGroup = &g;
		di.pMtl = &materials[mtlIndices[idx]];
		di.transform = transform;
		di.isValid = true;
	}
}

}
