#pragma once

#define RAPIDJSON_HAS_STDSTRING	1
#include "rapidjson/prettywriter.h"


namespace Ushuaia
{

	typedef rapidjson::PrettyWriter<rapidjson::StringBuffer>	Writer;

	int ftoa(float v, char *s, int m = 0, int n = 0);

	int ftoa(float *v, size_t len, char *s, int m = 0, int n = 0);

}
