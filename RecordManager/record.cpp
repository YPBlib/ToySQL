//
// Created by Aoleo on 2018/6/7.
//

#include "record.h"

#include <iostream>
#include <cstring>

using namespace std;

DynamicMemory::DynamicMemory(size_t size) {
    this->size = size;
    this->length = 0;
    this->ptr = malloc(size);
}
DynamicMemory::DynamicMemory(size_t size, void *buffer) {
    this->size = size;
    this->length = 0;
    this->ptr = buffer;
}
DynamicMemory::~DynamicMemory() {
    free(ptr);
}
bool DynamicMemory::put(void *source, size_t size) {
    size_t newSize = this->length + size;
    if(newSize > this->size){
        return false;
    } else{
        char * temp = (char *)this->ptr + length;
        this->length = newSize;
        memcpy(temp, source, size);
        return true;
    }
}
bool DynamicMemory::rewrite(void *source, size_t size, size_t offset) {
    if(size + offset > this->size){
        return false;
    }
    else{
        memcpy((char*)getPtr() + offset, source, size);
        return true;
    }
}
void* DynamicMemory::getPtr() {
    return this->ptr;
}
size_t DynamicMemory::getSize() {
    return this->size;
}
size_t DynamicMemory::getLength() {
    return this->length;
}

Attribute::Attribute(void *content, size_t size, DataType type) {
    this->size = size;
    this->type = type;
    this->content = content;
}

Tuple::Tuple(int id, string name) {
    this->id = id;
    this->name = name;
}
void Tuple::put(string attrKey, const Attribute &attrValue) {
    this->data.insert(pair<string, Attribute>(attrKey, attrValue));
}
DynamicMemory Tuple::serialize() {
    size_t sizeSum = 0;
    auto it = data.begin();
    while(it != data.end()){
        sizeSum += it->second.size;
        it++;
    }
    DynamicMemory memory(sizeSum);
    it = data.begin();
    while(it != data.end()){
        memory.put(it->second.content, it->second.size);
        it++;
    }
    return memory;
}
bool Tuple::loadData(DynamicMemory &memory) {
    if(data.empty()){
        return false;
    }
    char * temp = (char *) memory.getPtr();
    auto it = data.begin();
    while(it != data.end()){
        it->second.content = temp;
        temp += it->second.size;
        it++;
    }
    return true;
}

FileException::FileException(const char * errorMsg) : exception() {
    this->errorMsg = errorMsg;
}

const char *FileException::what() const noexcept {
    return this->errorMsg;
}
