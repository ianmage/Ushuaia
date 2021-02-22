#pragma once
#include "Entity.h"


namespace Ushuaia
{

struct Scene
{

	static Scene* pActive;

	std::unordered_map<size_t, Entity*> entities;

	Scene(std::string const & _name);
	virtual ~Scene();

	std::string const & Name() const { return name_; }

	void Serialize();
	bool Deserialize();

	Entity* CreateEntity(std::string const & _name);
	Entity* GetEntity(size_t _key) {
		auto const & itr = entities.find(_key);
			return (itr != entities.end()) ? itr->second : nullptr;
	}

private:
	std::string name_;

};

}
