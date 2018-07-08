#include "scene.h"
#include "../../cpp_common/commUtil.h"


namespace Ushuaia
{

Scene* Scene::pActive = nullptr;


Scene::Scene(std::string const & _name)
	: name(_name)
{
}


Scene::~Scene()
{
	for (auto & it : entities) {
		delete it.second;
	}
	entities.clear();
}


void Scene::serialize()
{
	JsonWriter writer;

	writer.Key("Entities");
	writer.StartObject();
	for (auto const & it : entities) {
		it.second->serialize(writer);
	}
	writer.EndObject();

	writer.save("scene/" + name);
}


bool Scene::deserialize()
{
	JsonReader reader;
	reader.load("scene/" + name);

	JsonValue::ConstMemberIterator itr = reader.FindMember("Entities");
	if (itr != reader.MemberEnd()) {
		for (auto const & m : itr->value.GetObject()) {
			char const * entName = m.name.GetString();
			size_t k = RT_HASH(entName);
			entNames_[k] = entName;
			Entity *pEnt = new Entity();
			entities[k] = pEnt;
			pEnt->deserialize(m.value);
		}
	}

	return true;
}


Entity* Scene::createEntity(std::string const & _name)
{
	size_t k = RT_HASH(_name.c_str());
	Entity *pEnt = getEntity(k);
	if (pEnt)
		return pEnt;

	pEnt = new Entity();
	entities.emplace(k, pEnt);
	entNames_.emplace(k, _name);

	return pEnt;
}

}
