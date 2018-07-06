#pragma once

#include "model.h"


namespace Ushuaia
{

	struct Entity
	{
		std::shared_ptr<Model> pModel;
		Matrix4x4 transform;
		std::string name;

		void serialize(Writer& writer) const;

		static std::unordered_map<size_t, Entity*> s_scnEnts;
		static std::unordered_map<size_t, Entity*> s_dynEnts;

		static Entity* create(std::string const & _name, bool _sceneObj = false);
		static Entity* get(size_t _nameKey, bool _sceneObj = false);

		static void fini();

	private:
		Entity(std::string const & _name);
	};

}
