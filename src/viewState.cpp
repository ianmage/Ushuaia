#include "viewState.h"


namespace Ushuaia
{

uint16_t RENDER_PASS::PP_ID_ = RENDER_PASS::GEO_NUM;
uint16_t const & RENDER_PASS::PP_ID = RENDER_PASS::PP_ID_;

uint16_t RENDER_PASS::NextPP()
{
	PP_ID_ = ((PP_ID_ - GEO_NUM) + 1) % 2 + GEO_NUM;
	return PP_ID_;
}

}
