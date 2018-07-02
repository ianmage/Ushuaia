#include "resData.h"
#include "entity.h"
#include <string>
#include "appConf.h"
#include "mathHelper.h"
#include "vtxDecl.h"
#include "../examples/common/bgfx_utils.h"


#define TEST	0


namespace My3D
{

bgfx::TextureHandle g_texFigure;
bgfx::TextureHandle g_texFlare;
bgfx::TextureHandle g_texFieldstone;

static Entity *g_pEntFloor;
uint8_t const numTrees = 8;
static Entity *g_pEntTrees[numTrees];
static Entity *g_pEntBunny, *g_pEntHollowcube, *g_pEntCube;

#if TEST
static Mesh g_meshTest;
static Entity *g_pEntTest;
#endif

static Mesh g_meshHPlane;
//static Mesh g_meshVPlane;
static Mesh g_meshTree;
static Mesh g_meshBunny;
static Mesh g_meshCube;
static Mesh g_meshHollowcube;

static Shader* g_pShaderDefault;
static Material* g_pMtlDefault;

static float timeAccuLight = 0.f;
static float timeAccuScene = 0.f;


void initResData()
{
	g_pShaderDefault = Shader::load("vs_exam", "fs_exam");

	g_pMtlDefault = new Material(g_pShaderDefault);
	g_pMtlDefault->renderStates = BGFX_STATE_CULL_CCW;

	Vector3 vS, vR, vT;

#if TEST
	PosColorVertex const cubeVert[] =
	{
		{ {-1.f,  1.f,  1.f}, 0xff000000 },
		{ { 1.f,  1.f,  1.f}, 0xff0000ff },
		{ {-1.f, -1.f,  1.f}, 0xff00ff00 },
		{ { 1.f, -1.f,  1.f}, 0xff00ffff },
		{ {-1.f,  1.f, -1.f}, 0xffff0000 },
		{ { 1.f,  1.f, -1.f}, 0xffff00ff },
		{ {-1.f, -1.f, -1.f}, 0xffffff00 },
		{ { 1.f, -1.f, -1.f}, 0xffffffff },
	};

	uint16_t const cubeTriList[] =
	{
		0, 1, 2, // 0
		1, 3, 2,
		4, 6, 5, // 2
		5, 6, 7,
		0, 2, 4, // 4
		4, 2, 6,
		1, 5, 3, // 6
		5, 7, 3,
		0, 4, 1, // 8
		4, 5, 1,
		2, 3, 6, // 10
		6, 3, 7,
	};

	g_meshTest.load(cubeVert, BX_COUNTOF(cubeVert),
		PosColorVertex::s_decl, cubeTriList, BX_COUNTOF(cubeTriList));

	g_pEntTest = Entity::create("Ent_Test", true);
	bx::mtxRotateXY(g_pEntTest->mtx.v, 0.f, 0.f);
	g_pEntTest->mtx.v[12] = -15.f;
	g_pEntTest->mtx.v[13] = -15.f;
	g_pEntTest->mtx.v[14] = 0.f;
	g_pEntTest->pModel = Model::create("Model_Test");
	g_pEntTest->pModel->pMesh = &g_meshTest;
	g_pEntTest->pModel->pMtl = new Material;
	g_pEntTest->pModel->pMtl->setMtlShader("vs_cubes", "fs_cubes");

	return;
#endif

	//Material::TextureState texSt;
	//texSt.hSampler = bgfx::createUniform("s_texColor", bgfx::UniformType::Int1);
	//g_defaultMtl.texStates.emplace_back(std::move(texSt));

	//g_texFigure = loadTexture("textures/figure-rgba.dds");
	//g_texFlare = loadTexture("textures/flare.dds");
	//g_texFieldstone = loadTexture("textures/fieldstone-rgba.dds");

	g_meshTree.load("meshes/tree.bin");
	g_meshBunny.load("meshes/bunny.bin");
	g_meshCube.load("meshes/cube.bin");
	g_meshHollowcube.load("meshes/hollowcube.bin");

	PosNormTC0Vertex const hPlaneVert[] =
	{
		{ { -1.0f, 0.0f,  1.0f }, encodeNormalRgba8(0.0f, 1.0f, 0.0f), { 5.f, 5.f } },
		{ { 1.0f, 0.0f,  1.0f }, encodeNormalRgba8(0.0f, 1.0f, 0.0f), { 5.f, 0.0f } },
		{ { -1.0f, 0.0f, -1.0f }, encodeNormalRgba8(0.0f, 1.0f, 0.0f), { 0.0f, 5.f } },
		{ { 1.0f, 0.0f, -1.0f }, encodeNormalRgba8(0.0f, 1.0f, 0.0f), { 0.0f, 0.0f } },
	};

	PosNormTC0Vertex const vPlaneVert[] =
	{
		{ { -1.0f,  1.0f, 0.0f }, encodeNormalRgba8(0.0f, 0.0f, -1.0f), { 1.0f, 1.0f } },
		{ { 1.0f,  1.0f, 0.0f }, encodeNormalRgba8(0.0f, 0.0f, -1.0f), { 1.0f, 0.0f } },
		{ { -1.0f, -1.0f, 0.0f }, encodeNormalRgba8(0.0f, 0.0f, -1.0f), { 0.0f, 1.0f } },
		{ { 1.0f, -1.0f, 0.0f }, encodeNormalRgba8(0.0f, 0.0f, -1.0f), { 0.0f, 0.0f } },
	};

	uint16_t const planeIndices[] =
	{
		0, 1, 2,
		1, 3, 2,
	};

	g_meshHPlane.load(hPlaneVert, BX_COUNTOF(hPlaneVert),
		PosNormTC0Vertex::s_decl, planeIndices, BX_COUNTOF(planeIndices));
	//g_meshVPlane.load(vPlaneVert, BX_COUNTOF(vPlaneVert),
	//	PosNormTC0Vertex::s_decl, planeIndices, BX_COUNTOF(planeIndices));

	g_pEntFloor = Entity::create("Ent_Floor", true);
	vS.Set(100.f, 100.f, 100.f);
	vR.Set(0.f, 0.f, 0.f);
	vT.Set(0.f, 0.f, 0.f);
	math::mtxSRT(g_pEntFloor->mtx, vS, vR, vT);
	g_pEntFloor->pModel = Model::create("Model_Floor");
	g_pEntFloor->pModel->pMesh = &g_meshHPlane;
	g_pEntFloor->pModel->pMtl = g_pMtlDefault;

	auto pModelTree = Model::create("Model_Tree");
	pModelTree->pMesh = &g_meshTree;
	pModelTree->pMtl = g_pMtlDefault;
	for (uint8_t i = 0; i < numTrees; ++i)
	{
		std::string namePrefix = "Ent_Tree_";
		g_pEntTrees[i] = Entity::create((namePrefix + std::to_string(i)).c_str(), true);
		g_pEntTrees[i]->pModel = pModelTree;
	
		float fI = static_cast<float>(i);
		vS.Set(2.f, 2.f, 2.f);
		vR.Set(0.f, fI, 0.f);
		float iAngle = fI * 2.f * bx::kPi / float(numTrees);
		vT.Set(bx::sin(iAngle) * 60.f, 0.f, bx::cos(iAngle) * 60.f);
		math::mtxSRT(g_pEntTrees[i]->mtx, vS, vR, vT);
	}
	
	g_pEntBunny = Entity::create("Ent_Bunny");
	g_pEntBunny->pModel = Model::create("Model_Bunny");
	g_pEntBunny->pModel->pMesh = &g_meshBunny;
	g_pEntBunny->pModel->pMtl = g_pMtlDefault;
	g_pEntHollowcube = Entity::create("Ent_Hollowcube");
	g_pEntHollowcube->pModel = Model::create("Model_Hollowcube");
	g_pEntHollowcube->pModel->pMesh = &g_meshHollowcube;
	g_pEntHollowcube->pModel->pMtl = g_pMtlDefault;
	g_pEntCube = Entity::create("Ent_Cube");
	g_pEntCube->pModel = Model::create("Model_Cube");
	g_pEntCube->pModel->pMesh = &g_meshCube;
	g_pEntCube->pModel->pMtl = g_pMtlDefault;
}


void finiResData()
{
	//bgfx::destroy(g_texFigure);
	//bgfx::destroy(g_texFlare);
	//bgfx::destroy(g_texFieldstone);
	delete g_pMtlDefault;

	g_meshHPlane.unload();
	//g_meshVPlane.unload();
	g_meshTree.unload();
	g_meshBunny.unload();
	g_meshCube.unload();
	g_meshHollowcube.unload();

#if TEST
	bgfx::destroy(g_pEntTest->pModel->pMtl->hShader);
	delete g_pEntTest->pModel->pMtl;
	g_meshTest.unload();
#endif
}


void updateResData()
{
#if TEST
	return;
#endif
	timeAccuLight += AppConf::deltaTime;
	timeAccuScene += AppConf::deltaTime;

	Vector3 vS, vR, vT;

	vS.Set(8.f, 8.f, 8.f);
	vR.Set(0.f, 1.56f - timeAccuScene, 0.f);
	vT.Set(30.f, 5.f, 0.f);
	math::mtxSRT(g_pEntBunny->mtx, vS, vR, vT);
	vS.Set(2.5f, 2.5f, 2.5f);
	vR.Set(0.f, 1.56f - timeAccuScene, 0.f);
	vT.Set(0.f, 10.f, 0.f);
	math::mtxSRT(g_pEntHollowcube->mtx, vS, vR, vT);
	vS.Set(2.5f, 2.5f, 2.5f);
	vR.Set(0.f, 1.56f - timeAccuScene, 0.f);
	vT.Set(-30.f, 5.f, 0.f);
	math::mtxSRT(g_pEntCube->mtx, vS, vR, vT);
}

}
