#pragma once

#define RAPIDJSON_HAS_STDSTRING	1
#define RAPIDJSON_HAS_CXX11_RVALUE_REFS	1
#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"
#include <vector>


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

std::string NumToAryStr(uint64_t num, uint8_t const ary, uint8_t const aryBegin);
uint64_t AryStrToNum(std::string const & s, uint8_t const ary, uint8_t const aryBegin);

}
