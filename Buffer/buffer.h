#pragma once
#ifndef LLVMSQL_BUFFER_H
#define LLVMSQL_BUFFER_H
#include "../Catalog/catalog.h"
#include <numeric>
#include <functional>
// queryable-record <=> buffer <=> DB files
template <typename T1 = unsigned int, typename T2 = bool>
class block
{
	std::pair<T1, T2> zata;
	unsigned int freq = 0u;
public:
	int series;  //series 指这一块是缓冲区的第几页
	bool isdirty = false;	// 根据record的反馈而变
	// 是record文件夹下的db文件
	string tbname;
	string filename;
	// 表示这一页上有多少record
	int recordnum = -1;
	// 表示这一页上每条record的大小
	int bytes = 0;
	// 表示距离文件头的距离
	int offset = 0;
	bool ispin = false;
	block(int t1, bool t2) :series(t1),isdirty(t2){}
	void writeback()
	{
		if (filename.length() == 0)
			throw runtime_error("filename.length()==0\n");
		char* tempuc = buff[series];
		FILE* wr = fopen(filename.c_str(), "rb+");
		fseek(wr, offset*BLOCK_8k, SEEK_SET);
		fwrite(tempuc, sizeof(unsigned char), BLOCK_8k, wr);
		fclose(wr);
		isdirty = false;
	}
	void updatefreq() { freq++; }
	const unsigned int getfreq()const { return this->freq; }
	
};

extern vector<block<>> BufferManager;

#endif // !LLVMSQL_BUFFER_H
