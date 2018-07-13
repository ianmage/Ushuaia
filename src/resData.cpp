#include "resData.h"
#include "scene.h"
#include <string>
#include "appConf.h"
#include "vtxDecl.h"
#include "../examples/common/bgfx_utils.h"
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

	PosNormTC0Vertex const hPlaneVert[] = {
		{ { -1.0f, 0.0f,  1.0f }, encodeNormalRgba8(0.0f, 1.0f, 0.0f),{ 5.f, 5.f } },
		{ { 1.0f, 0.0f,  1.0f }, encodeNormalRgba8(0.0f, 1.0f, 0.0f),{ 5.f, 0.0f } },
		{ { -1.0f, 0.0f, -1.0f }, encodeNormalRgba8(0.0f, 1.0f, 0.0f),{ 0.0f, 5.f } },
		{ { 1.0f, 0.0f, -1.0f }, encodeNormalRgba8(0.0f, 1.0f, 0.0f),{ 0.0f, 0.0f } },
	};

	uint16_t const planeIndices[] = {
		0, 1, 2,
		1, 3, 2,
	};

	auto pFloorEnt = pScene->GetEntity(CT_HASH("floor"));
	auto pFloorModel = pFloorEnt->pModel;
	assert(!pFloorModel->pMesh);
	pFloorModel->pMesh = Mesh::Create("floor", hPlaneVert, BX_COUNTOF(hPlaneVert),
		PosNormTC0Vertex::s_decl, planeIndices, BX_COUNTOF(planeIndices));
#if 0
	pFloorModel->pMtl = Material::Load("default");
#else
	Shader *pShader = Shader::Load("vs_clay", "fs_clay");
	Material * pMtl = new Material;
	pMtl->SetShader(pShader);
	pMtl->renderState = BGFX_STATE_CULL_CCW;
	pMtl->SetParamVec4(CT_HASH("u_mtlAlbedoMetal"), 1, 1, 1, 0);
	pMtl->SetParamVec4(CT_HASH("u_mtlNormalGloss"), 0, 0, 0, 0);
	pFloorModel->pMtl = pMtl;
#endif

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


void UpdateResData()
{
	timeAccuLight += AppConf::deltaTime;
	timeAccuScene += AppConf::deltaTime;

	Vector3 vS, vR, vT;

	vS.Set(8.f, 8.f, 8.f);
	vR.Set(0.f, 1.56f - timeAccuScene, 0.f);
	vT.Set(30.f, 5.f, 0.f);
	Entity::Get(CT_HASH("bunny"))->transform.SetSRT(vS, vR, vT);
	vS.Set(2.5f, 2.5f, 2.5f);
	vR.Set(0.f, 1.56f - timeAccuScene, 0.f);
	vT.Set(0.f, 10.f, 0.f);
	Entity::Get(CT_HASH("hollowcube"))->transform.SetSRT(vS, vR, vT);
	vS.Set(2.5f, 2.5f, 2.5f);
	vR.Set(0.f, 1.56f - timeAccuScene, 0.f);
	vT.Set(-30.f, 5.f, 0.f);
	Entity::Get(CT_HASH("cube"))->transform.SetSRT(vS, vR, vT);
}

}
