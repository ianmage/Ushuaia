#include "drawItem.h"
#include "entity.h"

//#pragma optimize("", off)

#define PACK_INST_DATA	1


namespace Ushuaia
{

	bool DrawChannel::s_supportInstancing = false;

	std::vector<DrawItem> DrawChannel::s_opaque;
	std::vector<InstanceItem> DrawChannel::s_instance;


	void DrawChannel::init()
	{
		s_supportInstancing = (0 !=
			(BGFX_CAPS_INSTANCING & bgfx::getCaps()->supported));
	}


	void DrawChannel::gather()
	{
		std::vector<Entity*> entArray;
		for (auto const & eIt : Entity::s_scnEnts)
		{
			entArray.push_back(eIt.second);
		}
		for (auto const & eIt : Entity::s_dynEnts)
		{
			entArray.push_back(eIt.second);
		}

		std::sort(entArray.begin(), entArray.end(),
			[](Entity *l, Entity *r)->bool{
			return l->pModel.get() < r->pModel.get(); }
		);

		if (s_supportInstancing)
		{
			for (size_t i = 0, arrEnd = entArray.size(); i < arrEnd;)
			{
				size_t j = i + 1;
				for (; j < arrEnd; ++j)
				{
					if (entArray[i]->pModel.get() != entArray[j]->pModel.get())
					{
						break;
					}
				}
				if (i + 3 < j) {
					DrawChannel::s_instance.emplace_back();
					auto & instItem = DrawChannel::s_instance.back();
					instItem.pModel = entArray[i]->pModel.get();
					for (size_t k = i; k < j; ++k)
						instItem.transforms.push_back(entArray[k]->transform);
				}
				else
				{
					DrawChannel::s_opaque.emplace_back();
					auto & opaItem = DrawChannel::s_opaque.back();
					opaItem.pModel = entArray[i]->pModel.get();
					opaItem.transform = entArray[i]->transform;
				}
				i = j;
			}
		}
		else // no support instancing
		{
			for (size_t i = 0, arrEnd = entArray.size(); i < arrEnd; ++i)
			{
				DrawChannel::s_opaque.emplace_back();
				auto & opaItem = DrawChannel::s_opaque.back();
				opaItem.pModel = entArray[i]->pModel.get();
				opaItem.transform = entArray[i]->transform;
			}
		}
	}


	void DrawChannel::drawOpaque(bgfx::ViewId viewId, uint64_t override0, uint64_t override1)
	{
		for (auto const & it : s_opaque)
		{
			bgfx::setTransform(it.transform.v);
			it.pModel->draw(viewId, override0, override1);
		}

		for (auto const & it : s_instance)
		{
#if PACK_INST_DATA
			uint16_t const instStride = static_cast<uint16_t>(sizeof(float) * 12);
#else
			uint16_t const instStride = static_cast<uint16_t>(sizeof(Matrix4x4));
#endif
			uint32_t const numInst = static_cast<uint32_t>(it.transforms.size());
			assert(numInst == bgfx::getAvailInstanceDataBuffer(numInst, instStride));
			bgfx::InstanceDataBuffer idb;
			bgfx::allocInstanceDataBuffer(&idb, numInst, instStride);
#if PACK_INST_DATA
			float *pData = reinterpret_cast<float*>(idb.data);
#else
			Matrix4x4 *pData = reinterpret_cast<Matrix4x4*>(idb.data);
#endif

			for (auto const & mtx : it.transforms)
			{
#if PACK_INST_DATA
				mtx.mtx4x3(pData);
				pData += 12;
#else
				*(pData++) = mtx;
#endif
			}

			auto pMtlShader = it.pModel->pMtl->pShader();
			Shader *pInstShader = Shader::load(
				pMtlShader->vsName() + "_instance", pMtlShader->fsName());
			assert(pInstShader);
			bgfx::setInstanceDataBuffer(&idb);
			it.pModel->draw(viewId, override0, override1, pInstShader);
		}
	}


	void DrawChannel::clearAll()
	{
		s_opaque.clear();
		s_instance.clear();
	}

}
