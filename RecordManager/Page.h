//
// Created by Aoleo on 2018/6/6.
//

#ifndef RECORDMANAGER_PAGE_H
#define RECORDMANAGER_PAGE_H

#include "Page.h"
#include "record.h"
#include <windows.h>
#include <string>
#include <exception>
#include <iostream>

#ifndef CONST_BLOCK_SIZE
#define BLOCK_SIZE (4096)
#endif

class Page {
private:
    int tableId;
    DynamicMemory memory;
    void* readFile(int tableId);

public:
    Page(int tableId);
};


#endif //RECORDMANAGER_PAGE_H
