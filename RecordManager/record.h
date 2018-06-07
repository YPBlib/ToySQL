//
// Created by Aoleo on 2018/6/7.
//

#ifndef RECORDMANAGER_RECORD_H
#define RECORDMANAGER_RECORD_H

#include <iostream>
#include <map>

using namespace std;

enum DataType {DB_INT, DB_FLOAT, DB_STRING};

class DynamicMemory {
private:
    size_t size;    /* The size allocated (bytes). */
    size_t length;    /* The actual room in use (bytes). */
    void * ptr;    /* The address of the segment. */
public:
    explicit DynamicMemory(size_t size);
    DynamicMemory(size_t size, void *buffer);

    DynamicMemory();

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

/**
 * @class Describe the attribute of a relation.
 * @example For SQL char(255), call Attribute attr(char*, 255, DB_STRING)
 */
class Attribute
{
public:
    size_t size;
    DataType type;
    void * content;
    /**
     * @param type Options: DB_STRING, DB_INT, DB_FLOAT.
     */
    Attribute( void * content, size_t size, DataType type);
};

class Tuple
{
private:
    int id;    /* The ID of the table which this tuple belongs to. */
    string name;    /* The table name. */
    map<string, Attribute> data;    /* Attributes data. */
public:
    Tuple(int id, string name);
    void put(string attrKey, const Attribute &attrValue);
    /* Fill data->second.content by binary information. */
    DynamicMemory serialize();
    /* Convert data to binary information. */
    bool loadData(DynamicMemory &memory);
};

class FileException : public exception {
private:
    const char * errorMsg;
public:
    explicit FileException(const char * errorMsg);

    const char *what() const noexcept override;
};

#endif //RECORDMANAGER_RECORD_H
