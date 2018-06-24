
#ifndef RECORDMANAGER_RECORD_H
#define RECORDMANAGER_RECORD_H

#include"../Buffer/buffer.h"

class tablerecord;
class record;

class DataValue
{
public:
	virtual const void* getValue() = 0;
	virtual ~DataValue() = default;
	virtual vector<char> emit_char() = 0;
};

class DataInt :public DataValue
{
public:
	int value = 0;
	DataInt(const int& value):value(value){}
	const void *getValue() { return (int*)&value; }
	~DataInt() = default;
	vector<char> emit_char()
	{
		unsigned int k = (unsigned int)value;
		vector<char> result;
		for (int i = 0; i != 4; ++i)
		{
			result.push_back(k | 0xff);
			k >>= 8;
		}
		return result;
	}
};

class DataDouble :public DataValue
{
public:
	double value = 0;
	DataDouble(const double& value) :value(value) {}
	const void *getValue() { return (double*)&value; }
	~DataDouble() = default;
	vector<char> result;
	vector<char> emit_char()
	{
		double d = value;
		double* ptrd = &d;
		char* pc = reinterpret_cast<char*>(ptrd);
		for (int i = 0; i < 8; ++i)
		{
			result.push_back(*(pc + i));
		}
		return result;
	}
};

class DataString :public DataValue
{
public:
	string value;
	DataString(char* value):value(value){}
	const void *getValue() { return (string*)&value; }
	~DataString() = default;
	vector<char> emit_char()
	{
		vector<char> result;
		for (auto i = 0; i != value.length(); ++i)
		{
			result.push_back(value[i]);
		}
		return result;
	}
};

class record
{
	using seg = std::pair<unsigned int, unsigned int>;
public:
	//  blk的坐标
	int pos = 0;
	//  在blk上的范围
	seg range;
	bool isdeleted = false;
	const tablerecord& tb;
	vector<shared_ptr<DataValue>> data;
	record(const int& pos, const seg& range, const tablerecord& tb, vector<shared_ptr<DataValue>> data) :
		pos(pos), range(range), tb(tb), data(std::move(data)) {}
};


// block to record
shared_ptr<DataValue> trans2record(int t, int n,unsigned int& curch,unsigned char* ptr)
{
	if(t==tok_INT)
	{
		int val = *reinterpret_cast<int*>(ptr+curch);
		curch += sizeof(int) / sizeof(unsigned char);
		return std::move(std::make_shared<DataInt>(val));
	}
	if (t == tok_CHAR)
	{
		char tpc[256]{ 0 };
		for (int i = 0; i < n; ++i)
		{
			tpc[i] = (char)*(ptr + curch + i);
		}
		curch += n;
		return std::move(std::make_shared<DataString>(tpc));
	}
	if (t == tok_DOUBLE || t == tok_FLOAT)
	{
		double val = *reinterpret_cast<double*>(ptr+curch);
		curch += sizeof(double) / sizeof(unsigned char);
		return std::move(std::make_shared<DataDouble>(val));
	}
	else
		throw runtime_error("wrong t in trans2record\n");
}

// record to block
void trans2block(unsigned char* dest, const vector<shared_ptr<DataValue>>& data, int bytes)
{
	int cnt=0;
	for (auto i : data)
	{
		auto vc = i->emit_char();
		for (auto c : vc)
		{
			dest[cnt++] = (unsigned char)c;
		}
	}
}


// 如何调度每个block中的可用空间(主要是删除产生的空洞，由于缓冲区较大，比替换算法更实用)
namespace recordspace
{
	struct singleFree
	{
		int blkpos;
		int offset;
		int bytes;
	};
	struct tbFreeList
	{
		int tbmap;
		vector<singleFree> list;
	};
	extern vector<tbFreeList> freelist;
}


class tablerecord
{
	using info = std::pair<int, string>;
	//using seg = std::tuple<unsigned int, unsigned int, unsigned int,unsigned int>;
	using seg = std::pair<std::pair<unsigned int, unsigned int>, std::pair< unsigned int, unsigned int>>;
public:
	catalog::SQLtable tbinfo;
	bool isdeleted = false;
	//  表中所有数据的位置加和
	vector<seg> range;
	vector<record> data;
	
	unsigned int cur = 0u;
	void initdata()
	{
		auto colinfo = tbinfo.cols;
		auto list = blockgen(tbinfo.name);
		unsigned int curch = 0u;
		for (auto i : list)
		{
			vector<shared_ptr<DataValue>> ptrs;
			for (auto j : colinfo)
			{
				auto t = j.coltype;
				auto n = j.N;
				ptrs.push_back(trans2record(t, n, curch, buff[i]));
			}
			data.push_back(record(i,std::make_pair<>(cur,cur+tbinfo.record_size),*this,ptrs));
		}
	}

	void computerange()
	{
		;
	}

	// 先清零
	void back_to_block()
	{
		// 统计 blk 下标
		vector<int> blk_index;
		for (auto i : data)
			blk_index.push_back(i.pos);
		std::sort(blk_index.begin(), blk_index.end());
		auto last_blk = std::unique(blk_index.begin(), blk_index.end());
		blk_index.erase(last_blk, blk_index.end());
		for (auto i : blk_index)
			std::memset(buff[i], 0, sizeof(char));
		for (auto i : data)
			trans2block(buff[i.pos]+i.range.first, i.data, i.tb.tbinfo.record_size);
	}

	// mark dirty, mark pin, 
	info select()
	{
		return info(0, "");
	}
	
	// mark dirty, mark pin, 
	info insert()
	{
		return info(0, "");
	}



	// mark dirty, mark pin, mark delete
	info dele()
	{
		return info(0, "");
	}
};

#endif //RECORDMANAGER_RECORD_H
