#include "Entity.h"


namespace Ushuaia
{

Entity::Entity(std::string const & _name)
 : pModel(nullptr), name_(_name)
{
}


void Entity::Serialize(JsonWriter & writer) const
{
	writer.StartObject();
	writer.Key("Model");
	writer.String(pModel->Name());
	writer.Key("Transform");
	WriteFloatArray(writer, transform.v, ArrayCount(transform));
	writer.EndObject();
}


bool Entity::Deserialize(JsonValue const & jsObj)
{
	JsonValue::ConstMemberIterator itr;

	itr = jsObj.FindMember("Model");
	if (itr != jsObj.MemberEnd()) {
		pModel = Model::Load(itr->value.GetString());
	}

	itr = jsObj.FindMember("Transform");
	if (itr != jsObj.MemberEnd()) {
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

	pEnt = new Entity(_name);
	s_entities.emplace(k, pEnt);

	return pEnt;
}


void Entity::Load(JsonValue const & jsObj)
{
	for (auto & m : jsObj.GetObject()) {
		Entity *pEnt = Create(m.name.GetString());
		pEnt->Deserialize(m.value);
	}
}


void Entity::Save(JsonWriter & writer)
{
	writer.StartObject();
	for (auto const & e : s_entities)
	{
		writer.String(e.second->name_);
		writer.StartObject();

		e.second->Serialize(writer);

		writer.EndObject();
	}
	writer.EndObject();
}


void Entity::Fini()
{
	for (auto & m : s_entities) {
		delete m.second;
	}
	s_entities.clear();
}

}
