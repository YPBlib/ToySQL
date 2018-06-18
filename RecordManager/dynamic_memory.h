//
// Created by Aoleo on 2018/6/13.
//

#ifndef RECORDMANAGER_DYNAMIC_MEMORY_H
#define RECORDMANAGER_DYNAMIC_MEMORY_H

#include <iostream>

using namespace std;

class DynamicMemory {
private:
    size_t size;    /* The size allocated (bytes). */
    size_t length;    /* The actual room in use (bytes). */
    void * ptr;    /* The address of the segment. */
public:
    explicit DynamicMemory(size_t size);
    DynamicMemory(size_t size, void *buffer);
    ~DynamicMemory();
    void * getPtr();
    size_t getSize();
    size_t getLength();
    /**
     * @brief To put something into memory.
     * @param source The address of the source.
     * @param size The size of the content which you want to save.
     * @return If memory is not enough, operation will be rejected and return false.
     */
    bool put(void* source, size_t size);
    bool rewrite(void* source, size_t size, size_t offset);
};


#endif //RECORDMANAGER_DYNAMIC_MEMORY_H
