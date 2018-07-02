#include "viewState.h"
#include "appConf.h"
#include "entity.h"
#include "resData.h"


namespace My3D
{

#define DEF_PROP(p)	decltype(p) p

	ViewState g_viewState;

	DEF_PROP(ViewState::texelHalf);
	DEF_PROP(ViewState::flipV);

	DEF_PROP(AppConf::mouseState);
	DEF_PROP(AppConf::deltaTime);

	DEF_PROP(Entity::s_scnEnts);
	DEF_PROP(Entity::s_dynEnts);

	DEF_PROP(Model::s_models);

}
