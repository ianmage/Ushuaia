#include "entity.h"
#include "../../cpp_common/commUtil.h"


namespace Ushuaia
{

	Entity* Entity::create(char const * name, bool sceneObj)
	{
		size_t id = RT_HASH(name);
		Entity* pEnt = nullptr;

		if (sceneObj)
		{
			auto const & it = s_scnEnts.find(id);
			if (it != s_scnEnts.end())
			{
				pEnt = it->second;
			}
			else
			{
				pEnt = new Entity;
				s_scnEnts.emplace(id, pEnt);
			}
		}
		else
		{
			auto const & it = s_dynEnts.find(id);
			if (it != s_dynEnts.end())
			{
				pEnt = it->second;
			}
			else
			{
				pEnt = new Entity;
				s_dynEnts.emplace(id, pEnt);
			}
		}

		return pEnt;
	}


	void Entity::fini()
	{
		for (auto & it : s_dynEnts)
		{
			delete it.second;
		}
		s_dynEnts.clear();
		for (auto & it : s_scnEnts)
		{
			delete it.second;
		}
		s_scnEnts.clear();
	}

}
