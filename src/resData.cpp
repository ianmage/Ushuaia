#include "resData.h"
#include "scene.h"
#include <string>
#include "appConf.h"
#include "vtxDecl.h"
#include "../examples/common/bgfx_utils.h"
#include "renderUtil.h"
#ifdef _APPLE
//#include <mach/mach.h>
#endif

#pragma optimize("", off)


namespace Ushuaia
{

static float timeAccuLight = 0.f;
static float timeAccuScene = 0.f;


void InitResData()
{
	Scene *pScene = new Scene("test");
	pScene->Deserialize();
	Scene::pActive = pScene;

	//uint64_t rs = 0x0000002000000000;
	Entity * pEnt = nullptr;
	Model * pModel = nullptr;
	Material * pMtl = nullptr;
	Color4F color;

	PosNormTC0Vertex const hPlaneVert[] = {
		{ { -1.0f, 0.0f,  1.0f }, ::encodeNormalRgba8(0.0f, 1.0f, 0.0f), { 5.f, 5.f } },
		{ { 1.0f, 0.0f,  1.0f }, ::encodeNormalRgba8(0.0f, 1.0f, 0.0f), { 5.f, 0.0f } },
		{ { -1.0f, 0.0f, -1.0f }, ::encodeNormalRgba8(0.0f, 1.0f, 0.0f), { 0.0f, 5.f } },
		{ { 1.0f, 0.0f, -1.0f }, ::encodeNormalRgba8(0.0f, 1.0f, 0.0f), { 0.0f, 0.0f } },
	};

	uint16_t const planeIndices[] = {
		0, 1, 2,
		1, 3, 2,
	};

	pEnt = pScene->GetEntity(CT_HASH("floor"));
	pModel = pEnt->pModel.get();
	assert(!pModel->pMesh);
	pModel->pMesh = Mesh::Create("floor", hPlaneVert, BX_COUNTOF(hPlaneVert),
		PosNormTC0Vertex::s_decl, planeIndices, BX_COUNTOF(planeIndices));
	pModel->materials.emplace_back();
	pMtl = &pModel->materials.back();
	pMtl->SetShader(Shader::Load("vs_tex", "fs_tex"));
	pMtl->renderState = BGFX_STATE_CULL_CCW;
	color.Set(1, 1, 1, 0);
	pMtl->SetParamColor(CT_HASH("PM_albedoMetal"), color);
	pMtl->SetParamFloat4(CT_HASH("PM_normalGloss"), 0, 1, 0, 0);
	TexState ts;
	ts.hSampler = bgfx::createUniform("S_albedoTex", bgfx::UniformType::Int1);
	ts.stage = 0;
	ts.pTex = TexMgr::LoadFromFile("figure-rgba");
	ts.flags = UINT32_MAX;
	pMtl->texStates.emplace_back(std::move(ts));

	pEnt = pScene->CreateEntity("sphere");
	pEnt->transform.SetSRT({1,1,1}, {0,0,0}, {0,10,0});
	pEnt->pModel = Model::Load("sphere");
	pModel = pEnt->pModel.get();
	std::vector<PosNormVertex> sphereVtx;
	std::vector<uint16_t> sphereIdx;
	CreateSphere(sphereVtx, sphereIdx, 2, 5.f);
	assert(!pModel->pMesh);
	pModel->pMesh = Mesh::Create("sphere", sphereVtx.data(), sphereVtx.size(),
		PosNormVertex::s_decl, sphereIdx.data(), sphereIdx.size());
	pModel->materials.emplace_back();
	pMtl = &pModel->materials.back();
	pMtl->SetShader(Shader::Load("vs_clay", "fs_clay"));
	pMtl->renderState = BGFX_STATE_CULL_CCW;
	color.SetUint(208, 90, 110, 0);
	ToLinear(color, color);
	pMtl->SetParamColor(CT_HASH("PM_albedoMetal"), color);
	pMtl->SetParamFloat4(CT_HASH("PM_normalGloss"), 0, 0, 0, 1);

	JsonReader reader;
	if (reader.Load("scene/dynamic")) {
		JsonValue::ConstMemberIterator itr = reader.FindMember("Entities");
		if (itr != reader.MemberEnd()) {
			for (auto const & m : itr->value.GetObject()) {
				char const * entName = m.name.GetString();
				auto pDynaEnt = Entity::Create(entName);
				pDynaEnt->Deserialize(m.value);
			}
		}
	}
}


void FiniResData()
{
	delete Scene::pActive;
}


static void PlaceObj(Vector3 & t, float ratio, float radius, float height)
{
	t.x = bx::cos(bx::kPi2 * ratio) * radius;
	t.y = height;
	t.z = bx::sin(bx::kPi2 * ratio) * radius;
}


void UpdateResData()
{
	timeAccuLight += AppConf::deltaTime;
	timeAccuScene += AppConf::deltaTime;

	Vector3 vS, vR, vT;

	vS.Set(8.f, 8.f, 8.f);
	vR.Set(0.f, 1.56f - timeAccuScene, 0.f);
	PlaceObj(vT, 1.f/3.f, 30.f, 3.f);
	Entity::Get(CT_HASH("bunny"))->transform.SetSRT(vS, vR, vT);
	vS.Set(2.5f, 2.5f, 2.5f);
	vR.Set(0.f, 1.56f - timeAccuScene, 0.f);
	PlaceObj(vT, 2.f/3.f, 30.f, 10.f);
	Entity::Get(CT_HASH("hollowcube"))->transform.SetSRT(vS, vR, vT);
	vS.Set(4.f, 4.f, 4.f);
	vR.Set(0.f, 1.56f - timeAccuScene, 0.f);
	PlaceObj(vT, 1.f, 30.f, 5.f);
	Entity::Get(CT_HASH("cube"))->transform.SetSRT(vS, vR, vT);
}

}
