#include "drawItem.h"
#include "scene.h"

//#pragma optimize("", off)

#define PACK_INST_DATA	1


namespace Ushuaia
{

	bool DrawChannel::s_supportInstancing = false;

	std::vector<DrawItem> DrawChannel::s_opaque;
	std::vector<InstanceItem> DrawChannel::s_instance;


	void DrawChannel::Init()
	{
		s_supportInstancing = (0 !=
			(BGFX_CAPS_INSTANCING & bgfx::getCaps()->supported));
	}


	void DrawChannel::Gather()
	{
		std::vector<Entity*> entArray;
		if (Scene::pActive) {
			for (auto const & eIt : Scene::pActive->entities) {
				entArray.push_back(eIt.second);
			}
		}
		for (auto const & eIt : Entity::s_entities) {
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


	void DrawChannel::DrawOpaque(bgfx::ViewId viewId, uint64_t override0, uint64_t override1)
	{
		for (auto const & m : s_opaque)
		{
			bgfx::setTransform(m.transform.v);
			m.pModel->Draw(viewId, override0, override1);
		}

		for (auto const & m : s_instance)
		{
#if PACK_INST_DATA
			uint16_t const instStride = static_cast<uint16_t>(sizeof(float) * 12);
#else
			uint16_t const instStride = static_cast<uint16_t>(sizeof(Matrix4x4));
#endif
			uint32_t const numInst = static_cast<uint32_t>(m.transforms.size());
			assert(numInst == bgfx::getAvailInstanceDataBuffer(numInst, instStride));
			bgfx::InstanceDataBuffer idb;
			bgfx::allocInstanceDataBuffer(&idb, numInst, instStride);
#if PACK_INST_DATA
			float *pData = reinterpret_cast<float*>(idb.data);
#else
			Matrix4x4 *pData = reinterpret_cast<Matrix4x4*>(idb.data);
#endif

			for (auto const & mtx : m.transforms)
			{
#if PACK_INST_DATA
				mtx.Mtx4x3(pData);
				pData += 12;
#else
				*(pData++) = mtx;
#endif
			}

			auto pMtlShader = m.pModel->pMtl->GetShader();
			Shader *pInstShader = Shader::Load(
				pMtlShader->VsName() + "_instance", pMtlShader->FsName());
			assert(pInstShader);
			bgfx::setInstanceDataBuffer(&idb);
			m.pModel->Draw(viewId, override0, override1, pInstShader);
		}
	}


	void DrawChannel::ClearAll()
	{
		s_opaque.clear();
		s_instance.clear();
	}

}
