#pragma once

#define RAPIDJSON_HAS_STDSTRING	1
#define RAPIDJSON_HAS_CXX11_RVALUE_REFS	1
#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"
#include <vector>
#include "math.h"
#include "../../cpp_common/commUtil.h"


namespace Ushuaia
{

typedef rapidjson::PrettyWriter<rapidjson::StringBuffer>	JsonWriterBase;
typedef rapidjson::Document	JsonReaderBase;
typedef rapidjson::Value	JsonValue;


struct JsonWriter : public JsonWriterBase
{
	rapidjson::StringBuffer strBuf;

	JsonWriter();

	void Save(std::string _fPath);
};


struct JsonReader : public JsonReaderBase
{
	//std::vector<char> content;

	bool Load(std::string _fPath);
};


int ftoa(float v, char *s, int m = 0, int n = 0);

int ftoa(float const *v, size_t len, char *s, int m = 0, int n = 0);

std::string NumToAry79Str(uint64_t num);
uint64_t Ary79StrToNum(std::string const & s);

inline void WriteFloatArray(JsonWriter & writer, float const * v, uint16_t cnt, uint8_t maxWidth = 12)
{
	std::vector<char> buf(maxWidth * cnt);
	size_t len = ftoa(v, cnt, buf.data(), 0, 4);
	writer.StartArray();
	writer.RawValue(buf.data(), len, rapidjson::kArrayType);
	writer.EndArray();
}

inline void ReadFloatArray(JsonValue const & jsObj, float * v)
{
	int i = 0;
	for (auto const & m : jsObj.GetArray()) {
		v[i++] = m.GetFloat();
	}
}

}
