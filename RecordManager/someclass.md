#### 定义类
- DynamicMemory: 其实就是包装了一下指针。
- Tuple: 一个元祖数据包，可以序列化为二进制，也可以从二进制生成数据。
- Attribute: 元组里面的一个属性，包括了值和一些信息。

```
#include <iostream>
#include <string>
#include <map>
#include <cstring>

#ifndef _BLOCK_SIZE_
#define BLOCK_SIZE (4096)
#endif

enum DataType {DB_INT, DB_FLOAT, DB_STRING};

using namespace std;


class DynamicMemory
{
private:
    size_t size;    /* The size allocated (bytes). */
    size_t length;    /* The actual room in use (bytes). */
    void * ptr;    /* The address of the segment. */
public:
    explicit DynamicMemory(size_t size){
        this->size = size;
        this->length = 0;
        this->ptr = malloc(size);
    }
    ~DynamicMemory(){
        free(ptr);
    }
    void * getPtr(){
        return ptr;
    }
    size_t getSize() const {
        return size;
    }

    size_t getLength() const {
        return length;
    }
    /**
     * @brief To put something into memory.
     * @param source The address of the source.
     * @param size The size of the content which you want to save.
     * @return If memory is not enough, operation will be rejected and return false.
     */
    bool put(void * source, size_t size){
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
    Attribute( void * content, size_t size, DataType type){
        this->size = size;
        this->type = type;
        this->content = content;
    }
};

class Tuple
{
private:
    int id;    /* The ID of the table which this tuple belongs to. */
    string name;    /* The table name. */
    map<string, Attribute> data;    /* Attributes data. */
public:
    Tuple(int id, string name){
        this->id = id;
        this->name = name;
    }
    void put(string attrKey, const Attribute &attrValue){
        this->data.insert(pair<string, Attribute>(attrKey, attrValue));
    }
    /* Fill data->second.content by binary information. */
    DynamicMemory serialize(){
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
    /* Convert data to binary information. */
    bool loadData(DynamicMemory &memory){
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
    }
};
```

#### 例子
```
int main() {
    Tuple previous(1, "table");
    //准备数据
    int age = 18;
    float gpa = 5.04;
    char name[10] = "XijunLiao";
    //产生属性
    Attribute attr_1(&age, sizeof(int), DB_INT);
    Attribute attr_2(name, 40, DB_STRING);
    Attribute attr_3(&gpa, sizeof(float), DB_FLOAT);

    previous.put(string("age"), attr_1);
    previous.put(string("name"), attr_2);
    previous.put(string("gpa"), attr_3);

    DynamicMemory memory = previous.serialize();

    Tuple now(1, "table");
    //产生属性模板
    Attribute attr_4(nullptr, sizeof(int), DB_INT);
    Attribute attr_5(nullptr, 40, DB_STRING);
    Attribute attr_6(nullptr, sizeof(float), DB_FLOAT);

    now.put(string("age"), attr_4);
    now.put(string("name"), attr_5);
    now.put(string("gpa"), attr_6);

    now.loadData(memory);

    return 0;
}

//运行结果：Tuple now是previous的一个copy。
```