#include "entity.h"
#include "../../cpp_common/commUtil.h"


namespace Ushuaia
{

	Entity::Entity(std::string const & _name)
		: name(_name)
	{
	}


	void Entity::serialize(Writer & writer) const
	{
		writer.String(name);
		writer.StartObject();

		writer.String("Model");
		pModel->serialize(writer);
		writer.String("Transform");
		size_t const maxFltWidth = 12;
		char buf[maxFltWidth * ArrayCount(transform)];
		int len = ftoa((float*)transform.v, ArrayCount(transform), buf, 0, 4);
		for (size_t i = 0; i < ArrayCount(transform); ++i)
		{
			writer.RawNumber(buf, len);
		}

		writer.EndObject();
	}


	Entity* Entity::get(size_t _nameKey, bool _sceneObj)
	{
		if (_sceneObj)
		{
			auto const & it = s_scnEnts.find(_nameKey);
			if (it != s_scnEnts.end())
				return it->second;
		}
		else
		{
			auto const & it = s_dynEnts.find(_nameKey);
			if (it != s_dynEnts.end())
				return it->second;
		}

		return nullptr;
	}


	Entity* Entity::create(std::string const & _name, bool _sceneObj)
	{
		size_t id = RT_HASH(_name.c_str());
		Entity* pEnt = get(id, _sceneObj);
		if (pEnt)
			return pEnt;

		pEnt = new Entity(_name);

		if (_sceneObj)
			s_scnEnts.emplace(id, pEnt);
		else
			s_dynEnts.emplace(id, pEnt);

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
