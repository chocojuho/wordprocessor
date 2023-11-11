#include "msvcfix.h"

#if ((_MSC_VER) <= 1600)
#include <string>
#include <cstdio>
using namespace std;

int stoi(string s)
{
	return atoi(s.c_str());
}

string to_string(int value)
{
	char s[18];
	string ret;
	sprintf(s, "%i", value);
	ret = s;
	return ret;
}
#endif

