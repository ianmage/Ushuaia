#include "serialize.h"
#include <stdio.h>


namespace Ushuaia
{

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


int ftoa(float *v, size_t cnt, char *s, int m, int n)
{
	char f[16];
	floatFmt(f, m, n);

	int ret = 0;
	int len = 0;
	len = sprintf(s, f, v[0]);
	ret += len;

	for (size_t i = 1; i < cnt; ++i)
	{
		s[len] = ',';
		s += len + 1;
		len = sprintf(s, f, v[i]);
		ret += len;
	}

	return ret;
}

}
