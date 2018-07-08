#include "serialize.h"
#include <stdio.h>
#include <fstream>
#include <boost/filesystem.hpp>

#pragma optimize("", off)


namespace Ushuaia
{

JsonWriter::JsonWriter()
 : JsonWriterBase(strBuf)
{
	SetIndent('\t', 1);
	StartObject();
}


void JsonWriter::save(std::string const & _fPath)
{
	EndObject();

	std::ofstream outFile(_fPath+".json", std::ios::out | std::ios::trunc);
	outFile << strBuf.GetString() << std::endl;
	outFile.close();
}


bool JsonReader::load(std::string const & _fPath)
{
	namespace fs = boost::filesystem;
	if (!fs::is_regular_file(_fPath))
		return false;

	std::ifstream ifs(_fPath+".json", std::ios::in | std::ios::binary);
	std::vector<char> content;
	content.resize(ifs.seekg(0, std::ios::end).tellg());
	ifs.seekg(0, std::ios::beg).read(content.data(), content.size());
	ifs.close();

	Parse(content.data());

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


int ftoa(float v, char *s, int m, int n)
{
	char f[16];
	floatFmt(f, m, n);

	return sprintf(s, f, v);
}


int ftoa(float const *v, size_t cnt, char *s, int m, int n)
{
	char f[16];
	floatFmt(f, m, n);

	int ret = 0;
	int len = 0;
	len = sprintf(s, f, v[0]) + 1;
	ret += len;

	for (size_t i = 1; i < cnt; ++i)
	{
		s[len-1] = ',';
		s += len;
		len = sprintf(s, f, v[i]) + 1;
		ret += len;
	}

	return ret - 1;
}

}
