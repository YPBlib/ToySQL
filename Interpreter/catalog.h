#pragma once


#ifndef llvmsql_catalog
#define llvmsql_catalog


#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include<exception>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include<exception>
#include"llvmsql.h"



class SQLtable
{
public:
	std::vector<int> intcol;
	std::vector<double> doublecol;
	std::vector<std::string> stringcol;
	std::string name;
};

class SQLcol
{
	;
};

class SQLintcol :public SQLcol
{
public:
	const int T = literal_int;
};

class SQLdoublecol :public SQLcol
{
public:
	const int T = literal_double;
};

template <std::size_t N>
class SQLcharcol :public SQLcol
{
public:
	const int sizet = N;
	const int T = literal_string;
};



































#endif
