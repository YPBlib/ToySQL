//
// Created by Aoleo on 2018/6/10.
//

#include <iostream>
#include "datatype.h"
#include "record.h"

using namespace std;

BaseData::BaseData(char *bytes, int size) : bytes(bytes), size(size) {}
DynamicMemory BaseData::serialize() {
    DynamicMemory memory(size);
    memory.put(bytes, size);
    return memory;
}
char *BaseData::getBytes() {
    return bytes;
}
int BaseData::getSize() {
    return size;
}

StringData::StringData(char *bytes, int size) : BaseData(bytes, size) {
    value = string(bytes, 0, size);
}
StringData::StringData(BaseData baseData) : BaseData(baseData.getBytes(), baseData.getSize()){
    value = string(baseData.getBytes(), 0, baseData.getSize());
}
string StringData::getString() {
    return value;
}

IntData::IntData(int i) : BaseData(nullptr, sizeof(int)) {
    value = i;
    this->bytes = (char *) &value;
}
IntData::getInt() {
    return value;
}


FloatData::FloatData(float f) : BaseData(nullptr, sizeof(float)) {
    value = f;
    this->bytes = (char *) &value;
}
float FloatData::getFloat() {
    return value;
}