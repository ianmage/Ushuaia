#include "Model.h"
#include "Serialize.h"
#include "DrawItem.h"


namespace Ushuaia
{

Model::Model(std::string const & _name)
	: pMesh(nullptr), name_(_name), mtlIndices_(0)
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
	if (pMesh && mtlIndices_ != 0) {
		writer.Key("MtlIndices");
		writer.StartArray();
		uint32_t mi = mtlIndices_;
		for (uint8_t i = 0; i < pMesh->groups.size(); ++i) {
			writer.Uint(mi & 0xF);
			mi >>= 4;
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
	assert(pMesh->groups.size() <= 8);
	mtlIndices_ = 0;
	itr = reader.FindMember("MtlIndices");
	if (itr != reader.MemberEnd()) {
		size_t const idxCnt = itr->value.Size();
		assert(idxCnt == pMesh->groups.size());
		uint8_t shift = 0;
		for (auto const & m : itr->value.GetArray()) {
			uint8_t mi = static_cast<uint8_t>(m.GetUint());
			mtlIndices_ |= mi << shift;
			shift += 4;
		}
	}

	return true;
}


bool Model::SetMtlIndices(std::vector<uint8_t> const & indices)
{
	size_t const idxCnt = indices.size();
	if (idxCnt != pMesh->groups.size())
		return false;
	mtlIndices_ = 0;
	uint8_t shift = 0;
	for (uint8_t i : indices) {
		mtlIndices_ |= i << shift;
		shift += 4;
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

	return pModel;
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

	if (mtlIndices_ == 0) {
		for (auto const & g : pMesh->groups) {
			DrawItem & di = DrawChannel::Add();
			di.primGroup = &g;
			di.pMtl = &materials[0];
			di.transform = transform;
			di.isValid = true;
		}
	}
	else {
		uint32_t mi = mtlIndices_;
		for (auto const & g : pMesh->groups) {
			DrawItem & di = DrawChannel::Add();
			di.primGroup = &g;
			di.pMtl = &materials[mi & 0xF];
			mi >>= 4;
			di.transform = transform;
			di.isValid = true;
		}
	}
}

}
