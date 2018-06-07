//
// Created by Aoleo on 2018/6/6.
//

#include "Page.h"
#include "record.h"
#include <windows.h>
#include <string>
#include <exception>
#include <iostream>

using namespace std;

Page::Page(int tableId):memory(BLOCK_SIZE, readFile(tableId)) {
    this->tableId = tableId;
}

void* Page::readFile(int tableId) {
    string fileName = "table_" + to_string(tableId);
    HANDLE handle = CreateFile(
            fileName.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL
            );
    if(handle == INVALID_HANDLE_VALUE){
        throw FileException("Failed opening file.");
    }
    DWORD fileSize = GetFileSize(handle, NULL);

    void * buffer = calloc(BLOCK_SIZE, 1);
    /* Init room if file has not been created. */
    DWORD writtenSize = 0;
    if(fileSize == 0){
        buffer = calloc(BLOCK_SIZE, 1);
        WriteFile(handle, buffer, BLOCK_SIZE, &writtenSize, NULL);
        free(buffer);
        buffer = NULL;
    }
    DWORD readSize = 0;
    ReadFile(handle, buffer, BLOCK_SIZE, &readSize, NULL);
    return buffer;
}
