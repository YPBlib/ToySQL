//
// Created by Aoleo on 2018/6/13.
//

#include <cstring>
#include "dynamic_memory.h"

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