#pragma once

#include "model.h"
#include "serialize.h"


namespace Ushuaia
{

	struct Entity
	{
		typedef std::unordered_map<size_t, Entity*>	EntMap;

		std::shared_ptr<Model> pModel;
		Matrix4x4 transform;

		void Serialize(JsonWriter & writer) const;
		bool Deserialize(JsonValue const & jsObj);

		static EntMap s_entities;
		static std::unordered_map<size_t, std::string> s_entNames;

		static Entity* Create(std::string const & _name);
		static Entity* Get(size_t key) {
			auto const & itr = s_entities.find(key);
			return (itr != s_entities.end()) ? itr->second : nullptr;
		}
		static void Load(JsonValue const & jsObj);
		static void Save(JsonWriter & writer);

		static void Fini();
	};

}
