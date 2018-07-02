#pragma once

#include "model.h"


namespace My3D
{

	struct Entity
	{
		std::shared_ptr<Model> pModel;
		Matrix4x4 mtx;

		static std::unordered_map<size_t, Entity*> s_scnEnts;
		static std::unordered_map<size_t, Entity*> s_dynEnts;

		static Entity* create(char const * name, bool sceneObj = false);

		static void fini();
	};

}
