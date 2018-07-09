#include "resData.h"
#include "scene.h"
#include <string>
#include "appConf.h"
#include "vtxDecl.h"
#include "../examples/common/bgfx_utils.h"
#include "../../cpp_common/commUtil.h"
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
	Light::ambLight.color.Set(0.1f, 0.1f, 0.2f, 0.f);
	Light::dirLight.color.Set(0.8f, 0.8f, 0.8f, 0.f);
	Light::dirLight.dir.Set(0.1f, -0.95f, 0.f, 0.f);

	Scene *pScene = new Scene("test");
	pScene->Deserialize();
	Scene::pActive = pScene;

	uint64_t num = 0x0000002000000000;
	uint8_t const aryBegin = 48;
	uint8_t const aryLast = 126;
	uint8_t const ary = aryLast - aryBegin + 1;
	std::string str = NumToAryStr(num, ary, aryBegin);
	uint64_t i = AryStrToNum(str, ary, aryBegin);
}


void FiniResData()
{
	delete Scene::pActive;
}


void UpdateResData()
{
	timeAccuLight += AppConf::deltaTime;
	timeAccuScene += AppConf::deltaTime;
}

}
