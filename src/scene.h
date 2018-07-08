#pragma once
#include "entity.h"


namespace Ushuaia
{

struct Scene
{

	static Scene* pActive;

	std::string name;
	std::unordered_map<size_t, Entity*> entities;

	Scene(std::string const & _name);
	~Scene();

	void serialize();
	bool deserialize();

	Entity* createEntity(std::string const & _name);
	Entity* getEntity(size_t _key) {
		auto const & itr = entities.find(_key);
			return (itr != entities.end()) ? itr->second : nullptr;
	}

private:
	std::unordered_map<size_t, std::string> entNames_;

};

}
