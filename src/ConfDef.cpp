#include "ViewState.h"
#include "AppConf.h"
#include "Entity.h"


namespace Ushuaia
{

#define DEF_PROP(p)	decltype(p) p

	ViewState g_viewState;

	DEF_PROP(ViewState::texelOffset);

	DEF_PROP(AppConf::mouseState);
	DEF_PROP(AppConf::deltaTime);

	DEF_PROP(Entity::s_entities);

	DEF_PROP(Model::s_models);

}
