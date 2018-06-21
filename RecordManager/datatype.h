//
// Created by Aoleo on 2018/6/10.
//

#ifndef RECORDMANAGER_DATATYPE_H
#define RECORDMANAGER_DATATYPE_H

#include <iostream>
//#include "datatype.h"
#include "dynamic_memory.h"

using namespace std;

class BaseData {
protected:
    char * bytes;
    int size;
public:
    virtual ~BaseData() = default;
    BaseData(char * bytes, int size);
    DynamicMemory serialize();
    char* getBytes();
    int getSize();
};

class StringData : public ::BaseData {
private:
    string value;
public:
    StringData(char * bytes, int size);
    StringData(BaseData baseData);
    string getString();
};


class IntData : public ::BaseData {
private:
    int value;
public:
    IntData(int i);
    int getInt();
};

class FloatData : public ::BaseData {
private:
    float value;
public:
    FloatData(float f);
    float getFloat();
};


#endif //RECORDMANAGER_DATATYPE_H
