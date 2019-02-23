#pragma once
#ifndef toysql_common
#define toysql_common

#include <unistd.h>
#include <cstdlib>
#include <algorithm>
#include <map>
#include <iostream>
#include <fstream>
#include <exception>
#include <regex>
#include <utility>
#include <memory>
#include <vector>
#include <string>
#include <cstdio>

namespace toysql
{
    extern std::string record_path;
    template <typename T>
    void exec(T ast);
}

#endif