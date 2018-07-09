#include "entity.h"
#include "../../cpp_common/commUtil.h"


namespace Ushuaia
{

decltype(Entity::s_entNames) Entity::s_entNames;


void Entity::Serialize(JsonWriter & _writer) const
{
	_writer.Key("Model");
	_writer.String(pModel->Name());
	_writer.Key("Transform");
	size_t const maxFltWidth = 12;
	char buf[maxFltWidth * ArrayCount(transform)];
	int len = ftoa(transform.v, ArrayCount(transform), buf, 0, 4);
	_writer.StartArray();
	_writer.RawValue(buf, len, rapidjson::kArrayType);
	_writer.EndArray();
}


bool Entity::Deserialize(JsonValue const & _jsObj)
{
	JsonValue::ConstMemberIterator itr;

	itr = _jsObj.FindMember("Model");
	if (itr != _jsObj.MemberEnd()) {
		pModel = Model::Load(itr->value.GetString());
	}

	itr = _jsObj.FindMember("Transform");
	if (itr != _jsObj.MemberEnd()) {
		int i = 0;
		for (auto & v : itr->value.GetArray()) {
			transform[i++] = v.GetFloat();
		}
	}
	else
		return false;

	return true;
}


Entity* Entity::Create(std::string const & _name)
{
	size_t k = RT_HASH(_name.c_str());
	Entity* pEnt = Get(k);
	if (pEnt)
		return pEnt;

	pEnt = new Entity();
	s_entities.emplace(k, pEnt);
	s_entNames.emplace(k, _name);

	return pEnt;
}


void Entity::Load(JsonValue const & _jsObj)
{
	for (auto & m : _jsObj.GetObject()) {
		Entity *pEnt = Create(m.name.GetString());
		pEnt->Deserialize(m.value);
	}
}


void Entity::Save(JsonWriter & _writer)
{
	_writer.StartObject();
	for (auto const & m : s_entNames)
	{
		_writer.String(m.second);
		_writer.StartObject();

		s_entities[m.first]->Serialize(_writer);

		_writer.EndObject();
	}
	_writer.EndObject();
}


void Entity::Fini()
{
	for (auto & m : s_entities) {
		delete m.second;
	}
	s_entities.clear();
}

}
