#pragma once
#ifndef toysql_common
#define toysql_common

#ifdef __linux__
#include <unistd.h>
#endif
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
#include <deque>
#include <string>
#include <cstring>
#include <cstdio>

namespace toysql
{
    extern std::string record_path;
    
    template <typename T>
    void exec(T ast)
    {
        std::cout<<"pass"<<std::endl;
    }
}

#endif