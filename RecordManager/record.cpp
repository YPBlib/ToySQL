#include "record.h"
#include "datatype.h"
#include <iostream>
#include <cstring>
#include <windows.h>
#include <string>
#include <exception>
#include <algorithm>



#ifndef _EMPTY_FLAG_
#define EMPTY_FLAG (-1)
#endif

using std::vector;
using std::string;

Table::Table(int id, const char* name, vector<Column> &columns) {
    this->id = id;
    this->name = name;
    this->attrSize = 0;
    for(auto &column : columns){
        addColumn(column);
    }
    this->nullBitMapSize = (columns.size() + 7) / 8;    //实现对8取整除数，例如8得1，9得2
    manager = new PageManager(this->id, this->attrSize, nullBitMapSize);
}
void Table::addColumn(Column &column) {
    columnNames.push_back(get<0>(column));
    this->columns.insert(make_pair<string, tuple<DataType, int, DataRule, int>>(
            get<0>(column),
            make_tuple(get<1>(column), get<2>(column), get<3>(column), (int)attrSize)
            ));
    this->attrSize += get<2>(column);
}

PageManager Table::getPageManager() {
    return *manager;
}

void Table::deleteRecord(string &attrName, bool (*check) (BaseData*)) {
    auto attr = (*this->columns.find(attrName)).second;
    int offset = get<ATTR_OFFSET>(attr);
    int size = get<ATTR_SIZE>(attr);
    auto list = manager->getQueryList();

    char * pos = nullptr;
    BaseData *data = nullptr;
    for(auto &it : list){
        pos = manager->getPosition(it);
        data = new BaseData(pos+offset, size);
        if(check(data)){
            manager->erase(it);
        }
    }

}

/**
 * @note 确保查询前已经检查了属性名的合法性
 * @param attrName
 * @return
 */
vector<map<string, BaseData*>> Table::queryAll(vector<string> &attrNames) {
    vector<map<string, BaseData*>> results;
    auto list = manager->getRePosAll();
    int off = 0;
    int size = 0;
    for(auto it : list){
        map<string, BaseData*> re;
        for(auto attrName : attrNames){
            //判断空位图
            auto nullAttrs = getNullAttr(it + attrSize);
            if(find(nullAttrs.begin(), nullAttrs.end(), attrName) != nullAttrs.end()){
                re.insert(pair<string, BaseData*>(attrName, nullptr));
                continue;
            }
            else {
                off = get<ATTR_OFFSET>((*columns.find(attrName)).second);
                size = get<ATTR_SIZE>((*columns.find(attrName)).second);
                switch (get<ATTR_TYPE>((*columns.find(attrName)).second)) {
                    case DB_STRING: {
                        re.insert(pair<string, BaseData*>(attrName, new StringData(it+off, size)));
                        break;
                    }
                    case DB_INT: {
                        re.insert(pair<string, BaseData*>(attrName, new IntData(GET_INT(it+off))));
                        break;
                    }
                    case DB_FLOAT: {
                        re.insert(pair<string, BaseData*>(attrName, new FloatData(GET_FLOAT(it+off))));
                        break;
                    }
                    default:
                        break;
                }
            }
        }

        results.push_back(re);
    }
    return results;
}

/**
 * 对所有属性进行插入。
 * @note 必须先检查过是否所有属性都制定了值，以及主键合法性（不能为NULL）。
 * @param record
 * @return
 */
int Table::insertRecord(vector<char*> &record) {
    //检查主键以及unique
    auto pos = this->manager->getRePosAll();    //所有记录的首地址
    int off = 0;    //用于记录所有具有唯一性属性的偏移量
    string name;    //临时保存当前比较属性的名字
    bool isMatch = false;    //用来表示是否匹配到一个相同
    for(int i = 0; i < this->columnNames.size(); i++){
        name = columnNames.at(i);
        //如果属性被指定了唯一性，就要进行检查，如果传入的属性的内容是NULL，对于UNIQUE可以跳过检查
        if(get<ATTR_RULE>((*this->columns.find(name)).second) != NONE && record.at(i) != nullptr){

            off = get<ATTR_OFFSET>((*this->columns.find(name)).second);
            for(auto base : pos){
                if(memcmp(base+off, record.at(i), get<ATTR_SIZE>((*this->columns.find(name)).second)) == 0){
                    isMatch = true;
                    break;    //一旦比较到一个相同项，立刻退出
                }
            }
        }
        if(isMatch){
            break;
        }
    }
    //违反主键约束插入本次插入失败，否则执行插入
    if(isMatch){
        return 0;
    }
    //执行插入，应该调用PageManager的接口，直接操作内存存在页面超出的情况
    DynamicMemory memory(attrSize + nullBitMapSize);
    char* nullBitMap = (char*) calloc(nullBitMapSize, 1);

    char* p;
    for(int i = 0; i < this->columnNames.size(); i++){
        name = columnNames.at(i);
        p = record.at(i);
        //如果P是空指针，修改空位图，否则直接执行内存拷贝
        if(p == nullptr){
            setBitMap(nullBitMap, nullBitMapSize, i);
        } else{
            memory.rewrite(
                    p,
                    get<ATTR_SIZE>((*this->columns.find(name)).second),
                    get<ATTR_OFFSET>((*this->columns.find(name)).second)
            );
        }
        //追加空位图
        memory.rewrite(
                nullBitMap,
                nullBitMapSize,
                attrSize
        );
    }
    manager->insert(memory);
    return 1;
}

int Table::insertRecord(map<string, char *> record) {
    vector<char*> allAttr;
    for(auto &it : columnNames){
        if(record.find(it) == record.end()){
            allAttr.push_back(nullptr);
        }
        else {
            allAttr.push_back((*record.find(it)).second);
        }
    }
    return insertRecord(allAttr);
}

vector<char *> Table::queryAllPri(string &attrName) {
    auto pos = this->manager->getRePosAll();
    int offset = get<ATTR_OFFSET>((*this->columns.find(attrName)).second);
    for(auto &it : pos){
        it += offset;
    }
    return pos;
}


void Table::setBitMap(char * source, int size, int position){
    if(position >= size * 8){
        return;
    }
    int unit = position / 8;
    int off = position % 8;
    unsigned char x = 0x80;
    source[unit] ^= (x >> off);
}

vector<int> Table::getNullPos(char* bitMap){
    vector<int> pos;
    unsigned char x = 0;
    for(int i = 0; i < nullBitMapSize; i++){
        x = (unsigned char) bitMap[i];
        for(int j = 0; j < 8; j++){
            if( (x & 0x80) != 0 ){
                pos.push_back(j + i * 8);
            }
            x = x << 1;
        }
    }
}
vector<string> Table::getNullAttr(char* bitMap){
    vector<string> pos;
    unsigned char x = 0;
    for(int i = 0; i < nullBitMapSize; i++){
        x = (unsigned char) bitMap[i];
        for(int j = 0; j < 8; j++){
            if( (x & 0x80) != 0 ){
                pos.push_back(columnNames.at(j + i * 8));
            }
            x = x << 1;
        }
    }
}