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

		void serialize(JsonWriter & _writer) const;
		bool deserialize(JsonValue const & _jsObj);

		static EntMap s_entities;
		static std::unordered_map<size_t, std::string> s_entNames;

		static Entity* create(std::string const & _name);
		static Entity* get(size_t _key) {
			auto const & itr = s_entities.find(_key);
			return (itr != s_entities.end()) ? itr->second : nullptr;
		}
		static void load(JsonValue const & _jsObj);
		static void save(JsonWriter & _writer);

		static void fini();
	};

}
