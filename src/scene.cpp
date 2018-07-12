#include "scene.h"
#include "light.h"


namespace Ushuaia
{

Scene* Scene::pActive = nullptr;


Scene::Scene(std::string const & _name)
	: name_(_name)
{
}


Scene::~Scene()
{
	for (auto & m : entities) {
		delete m.second;
	}
	entities.clear();
}


void Scene::Serialize()
{
	JsonWriter writer;

	writer.Key("Lights");
	Light::Serialize(writer);

	writer.Key("Entities");
	writer.StartObject();
	for (auto const & m : entities) {
		m.second->Serialize(writer);
	}
	writer.EndObject();

	writer.Save("scene/" + name_);
}


bool Scene::Deserialize()
{
	JsonReader reader;
	if (!reader.Load("scene/" + name_))
		return false;

	JsonValue::ConstMemberIterator itr;

	itr = reader.FindMember("Lights");
	if (itr != reader.MemberEnd()) {
		Light::Deserialize(itr->value);
	}

	itr = reader.FindMember("Entities");
	if (itr != reader.MemberEnd()) {
		for (auto const & m : itr->value.GetObject()) {
			char const * entName = m.name.GetString();
			size_t k = RT_HASH(entName);
			entNames_[k] = entName;
			Entity *pEnt = new Entity();
			entities[k] = pEnt;
			pEnt->Deserialize(m.value);
		}
	}

	return true;
}


Entity* Scene::CreateEntity(std::string const & _name)
{
	size_t k = RT_HASH(_name.c_str());
	Entity *pEnt = GetEntity(k);
	if (pEnt)
		return pEnt;

	pEnt = new Entity();
	entities.emplace(k, pEnt);
	entNames_.emplace(k, _name);

	return pEnt;
}

}
