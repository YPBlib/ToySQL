#pragma once
#ifndef LLVMSQL_BUFFER_H
#define LLVMSQL_BUFFER_H
#include"../Interpreter/llvmsql.h"





// examine whether no record is larger than a block
// read write conflict

// query <=> record <=> buffer <=> DB files
class block
{
public:
	const string filename;
	const unsigned int offset;
	bool isdirty = false;
	bool ispin = false;
	shared_ptr<unsigned char[BUFFER_BLOCK_SIZE]> data = nullptr;
	block(const string& filename,const unsigned int& offset) :filename(filename), offset(offset)
	{
		/*fstream fs(filename);
		fs.seekp(offset);
*/
		FILE* r=fopen(filename.c_str(),"rb");
		data = make_shared<unsigned char[BUFFER_BLOCK_SIZE]>();
		std::fseek(r, offset, SEEK_SET);
		fread(*data.get(), BUFFER_BLOCK_SIZE, sizeof(unsigned char), r);
	}
	void writeback()
	{
		;
	}
};


// lru
// mru




/*
class PageManager {
public:
	int id;    ///< 表ID
	int attrSize;    ///< 定长数据的总大小
	int nullBitMapSize;    ///< 空位图大小
	int recordSize;    ///< 数据+空位图的总大小
	int slotsHead;    ///< 头页能储存的最大记录数
	int slotsBody;    ///< 普通页能储存的最大记录数
	vector<char *> pages;    ///< 所有的页，默认初始化时全部写入缓冲区，调用flush后将全部写入磁盘
	bool isQueryListLoad;    ///< 用于标记是否存在查询列表以实现复用
	vector<int> queryList;    ///< 查询列表，即保存所有有效的记录的插槽位
	vector<int> gapList;    ///< 空闲列表
	void initGapList();    ///< 初始化空闲列表，在构造方法中调用
	void requestPage();    ///< 向buffer manager申请内存页
	int extract(int pageIndex, int offset);
	void locateSlot(int slotIndex, int* pageIndex, int* offset);
	PageManager(int id, int attrSize, int nullBitMapSize);
	void insert(DynamicMemory& buffer);
	void initQueryList();
	void erase(int slotIndex);
	void flush();
	vector<int> &getQueryList();
	char* getPosition(int slotIndex);
	vector<char *> getRePosAll();
};
*/


#endif // !LLVMSQL_BUFFER_H
