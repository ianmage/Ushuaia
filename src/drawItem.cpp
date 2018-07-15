#include "drawItem.h"
#include "scene.h"

//#pragma optimize("", off)

#define PACK_INST_DATA	1


namespace Ushuaia
{

void DrawUnit::Submit(bgfx::ViewId vId,
		uint64_t overrideSt0, uint64_t overrideSt1) const
{
	bgfx::setState(RenderState::overrideMe(pMtl->renderState, overrideSt0, overrideSt1));
	
	if (bgfx::isValid(primGroup->hVB))
		bgfx::setIndexBuffer(primGroup->hIB);
	bgfx::setVertexBuffer(0, primGroup->hVB);
}


void DrawItem::Submit(bgfx::ViewId vId,
	uint64_t overrideSt0, uint64_t overrideSt1) const
{
	bgfx::setTransform(transform.v);

	pMtl->SubmitParams();

	DrawUnit::Submit(vId, overrideSt0, overrideSt1);

	bgfx::submit(vId, pMtl->GetShader()->hProgram);
}


struct DrawItemLtCmp
{
	inline bool operator()(DrawItem const & lhs, DrawItem const & rhs) const {
		return lhs.pMtl < rhs.pMtl
			&& lhs.primGroup < rhs.primGroup;
	}
};


inline bool DrawItemEqCmp(DrawItem const & lhs, DrawItem const & rhs)
{
	return lhs.pMtl == rhs.pMtl &&
		lhs.primGroup == rhs.primGroup;
}


void InstanceItem::Submit(bgfx::ViewId vId,
	uint64_t overrideSt0, uint64_t overrideSt1) const
{
#if PACK_INST_DATA
	uint16_t const instStride = static_cast<uint16_t>(sizeof(float) * 12);
#else
	uint16_t const instStride = static_cast<uint16_t>(sizeof(Matrix4x4));
#endif
	uint32_t const numInst = static_cast<uint32_t>(transforms.size());
	assert(numInst == bgfx::getAvailInstanceDataBuffer(numInst, instStride));
	bgfx::InstanceDataBuffer idb;
	bgfx::allocInstanceDataBuffer(&idb, numInst, instStride);
#if PACK_INST_DATA
	float *pData = reinterpret_cast<float*>(idb.data);
#else
	Matrix4x4 *pData = reinterpret_cast<Matrix4x4*>(idb.data);
#endif

	for (auto const & mtx : transforms)
	{
#if PACK_INST_DATA
		mtx.Mtx4x3(pData);
		pData += 12;
#else
		*(pData++) = mtx;
#endif
	}

	bgfx::setInstanceDataBuffer(&idb);

	pMtl->SubmitParams(pInstShader);

	DrawUnit::Submit(vId, overrideSt0, overrideSt1);

	bgfx::submit(vId, pInstShader->hProgram);
}
	

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
	if (Scene::pActive) {
		for (auto const & e : Scene::pActive->entities) {
			e.second->pModel->Draw(e.second->transform);
		}
	}
	for (auto const & e : Entity::s_entities) {
		e.second->pModel->Draw(e.second->transform);
	}

	std::sort(s_opaque.begin(), s_opaque.end(), DrawItemLtCmp());

	if (s_supportInstancing)
	{
		for (uint32_t i = 0, arrEnd = s_opaque.size(); i < arrEnd;)
		{
			uint32_t j = i + 1;
			for (; j < arrEnd; ++j)
			{
				if (!DrawItemEqCmp(s_opaque[i], s_opaque[j]))
					break;
			}
			if (i + 3 < j) {
				s_instance.emplace_back();
				auto & instItem = s_instance.back();
				instItem = s_opaque[i];
				for (size_t k = i; k < j; ++k) {
					instItem.transforms.push_back(s_opaque[k].transform);
					s_opaque[k].isValid = false;
				}
			}
			else
			{
				s_opaque[i].isValid = true;
			}
			i = j;
		}
	}
	else // no support instancing
	{
		for (uint32_t i = 0, arrEnd = s_opaque.size(); i < arrEnd; ++i)
		{
			s_opaque[i].isValid = true;
		}
	}
}


DrawItem& DrawChannel::Add()
{
	s_opaque.emplace_back();
	return s_opaque.back();
}


void DrawChannel::DrawOpaque(bgfx::ViewId viewId, uint64_t override0, uint64_t override1)
{
	for (auto const & m : s_opaque)
	{
		m.Submit(viewId, override0, override1);
	}

	for (auto const & m : s_instance)
	{
		m.Submit(viewId, override0, override1);
	}
}


void DrawChannel::ClearAll()
{
	s_opaque.clear();
	s_instance.clear();
}

}
