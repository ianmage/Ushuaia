#pragma once

#include "model.h"
#include "serialize.h"


namespace Ushuaia
{

	struct Entity
	{
	public:
		std::shared_ptr<Model> pModel;
		Matrix4x4 transform;

		Entity(std::string const & _name);

		std::string const & Name() const { return name_; }

		void Serialize(JsonWriter & writer) const;
		bool Deserialize(JsonValue const & jsObj);

		static std::unordered_map<size_t, Entity*> s_entities;

		static Entity* Create(std::string const & _name);
		static Entity* Get(size_t key) {
			auto const & itr = s_entities.find(key);
			return (itr != s_entities.end()) ? itr->second : nullptr;
		}
		static void Load(JsonValue const & jsObj);
		static void Save(JsonWriter & writer);

		static void Fini();

	private:
		std::string name_;
	};

}
