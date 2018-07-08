#include "entity.h"
#include "../../cpp_common/commUtil.h"


namespace Ushuaia
{

decltype(Entity::s_entNames) Entity::s_entNames;


void Entity::serialize(JsonWriter & _writer) const
{
	_writer.Key("Model");
	_writer.String(pModel->name);
	_writer.Key("Transform");
	size_t const maxFltWidth = 12;
	char buf[maxFltWidth * ArrayCount(transform)];
	int len = ftoa(transform.v, ArrayCount(transform), buf, 0, 4);
	_writer.StartArray();
	_writer.RawValue(buf, len, rapidjson::kArrayType);
	_writer.EndArray();
}


bool Entity::deserialize(JsonValue const & _jsObj)
{
	JsonValue::ConstMemberIterator itr;

	itr = _jsObj.FindMember("Model");
	if (itr != _jsObj.MemberEnd()) {
		pModel = Model::load(itr->value.GetString());
	}

	itr = _jsObj.FindMember("Transform");
	if (itr != _jsObj.MemberEnd()) {
		int i = 0;
		for (auto & v : itr->value.GetArray()) {
			transform[i++] = v.GetFloat();
		}
	}

	return true;
}


Entity* Entity::create(std::string const & _name)
{
	size_t k = RT_HASH(_name.c_str());
	Entity* pEnt = get(k);
	if (pEnt)
		return pEnt;

	pEnt = new Entity();
	s_entities.emplace(k, pEnt);
	s_entNames.emplace(k, _name);

	return pEnt;
}


void Entity::load(JsonValue const & _jsObj)
{
	for (auto & m : _jsObj.GetObject()) {
		Entity *pEnt = create(m.name.GetString());
		pEnt->deserialize(m.value);
	}
}


void Entity::save(JsonWriter & _writer)
{
	_writer.StartObject();
	for (auto const & it : s_entNames)
	{
		_writer.String(it.second);
		_writer.StartObject();

		s_entities[it.first]->serialize(_writer);

		_writer.EndObject();
	}
	_writer.EndObject();
}


void Entity::fini()
{
	for (auto & it : s_entities) {
		delete it.second;
	}
	s_entities.clear();
}

}
