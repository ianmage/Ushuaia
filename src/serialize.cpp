#include "serialize.h"
#include <stdio.h>
#include <boost/filesystem.hpp>

//#pragma optimize("", off)


namespace Ushuaia
{

JsonWriter::JsonWriter()
 : JsonWriterBase(strBuf)
{
	SetIndent('\t', 1);
	StartObject();
}


void JsonWriter::Save(std::string _fPath)
{
	_fPath += ".json";

	EndObject();

	::FILE *pFile = ::fopen(_fPath.c_str(), "wb");
	::fwrite(strBuf.GetString(), sizeof(decltype(strBuf)::Ch), strBuf.GetLength(), pFile);
	::fclose(pFile);
}


bool JsonReader::Load(std::string _fPath)
{
	_fPath += ".json";

	namespace fs = boost::filesystem;
	if (!fs::is_regular_file(_fPath))
		return false;

	::FILE *pFile = ::fopen(_fPath.c_str(), "rb");
	::fseek(pFile, 0, SEEK_END);
	long fSize = ::ftell(pFile);
	::rewind(pFile);

	std::vector<char> content;
	content.resize(sizeof(char) * fSize + 1);
	size_t result = ::fread(content.data(), sizeof(char), fSize, pFile);
	assert(result == fSize);
	::fclose(pFile);
	content.back() = '\0';

	if (Parse(content.data()).HasParseError()) {
#ifdef _DEBUG
		pFile = ::fopen("r:/test.json", "wb");
		::fwrite(content.data(), sizeof(char), content.size(), pFile);
		::fclose(pFile);
		assert(false);
#endif
		return false;
	}

	return true;
}


static void floatFmt(char *f, int m, int n)
{
	if (!m && !n) {
		strcpy(f, "%f");
	}
	else if (!m && n) {
		sprintf(f, "%%.%df", n);
	}
	else if (m && !n) {
		sprintf(f, "%%%df", m);
	}
	else {
		sprintf(f, "%%%d.%df", m, n);
	}
}


int trimTail(char *s, int len, char const c)
{
	while (s[len - 1] == c) {
		if (len < 2)
			break;
		--len;
	}
	s[len] = 0;
	return len;
}


int ftoa(float v, char *s, int m, int n)
{
	char f[16];
	floatFmt(f, m, n);

	int len = sprintf(s, f, v);
	len = trimTail(s, len, '0');
	return len;
}


int ftoa(float const *v, size_t cnt, char *s, int m, int n)
{
	char f[16];
	floatFmt(f, m, n);

	int ret = 0;
	int len = 0;
	len = sprintf(s, f, v[0]);
	len = trimTail(s, len, '0');
	++len;	// for ,
	ret += len;

	for (size_t i = 1; i < cnt; ++i)
	{
		s[len-1] = ',';
		s += len;
		len = sprintf(s, f, v[i]);
		len = trimTail(s, len, '0');
		++len;	// for ,
		ret += len;
	}

	return ret - 1;
}


static size_t CalcAryLength(uint64_t num, uint8_t const ary)
{
	size_t ret = 1;
	while (num >= ary) {
		num /= ary;
		++ret;
	};
	return ret;
}

std::string NumToAry79Str(uint64_t num)	// base on ascii visable char
{
	uint8_t const aryBegin = 48;
	uint8_t const ary = 79;

	std::string s;
	s.resize(CalcAryLength(num, ary));

	size_t idx = s.size() - 1;
	while (num >= ary) {
		s[idx--] = (num % ary) + aryBegin;
		num /= ary;
	};
	assert(idx == 0);
	s[0] = (uint8_t)num + aryBegin;

	return s;
}


uint64_t Ary79StrToNum(std::string const & s)
{
	uint8_t const aryBegin = 48;
	uint8_t const ary = 79;
	
	uint64_t n = 0;

	uint64_t powBase = 1;
	for (size_t i = s.size()-1; i > 0; --i) {
		n += (s[i] - aryBegin) * powBase;
		powBase *= ary;
	}
	n += (s[0] - aryBegin) * powBase;

	return n;
}

}
