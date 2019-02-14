
#ifndef RECORDMANAGER_RECORD_H
#define RECORDMANAGER_RECORD_H

#include "../Buffer/buffer.h"

//class tablerecord;
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
		int k = value;
		int* pk = &k;
		char* pc = reinterpret_cast<char*>(pk);
		vector<char> result;
		for (int i = 0; i != 4; ++i)
		{
			result.push_back(pc[i]);
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
			result.push_back(pc[i]);
		}
		return result;
	}
};

class DataString :public DataValue
{
public:
	string value;
	DataString(string value) :value(value) {}
	//DataString(char* value):value(value){}
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
public:
	//  blk������
	int pos = 0;
	//  ����ҳ�ϵĵڼ���,��0��ʼ
	int series = -1;
	// record ��С
	int size = -1;
	bool isdeleted = false;
	vector<shared_ptr<DataValue>> data;
	record(const int& pos, const int & series,const int& size, vector<shared_ptr<DataValue>> data) :
		pos(pos), series(series), size(size), data(std::move(data)) {}
};

// ��ε���ÿ��block�еĿ��ÿռ�(��Ҫ��ɾ�������Ŀն������ڻ������ϴ󣬱��滻�㷨��ʵ��)
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

/*
class tablerecord
{
	using seg = std::pair<std::pair<unsigned int, unsigned int>, std::pair< unsigned int, unsigned int>>;
public:

	vector<record> data;

	void initdata()
	{
		auto colinfo = tbinfo.cols;
		auto list = blockgen(tbinfo.tbname);
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

	// ������
	void back_to_block()
	{
		// ͳ�� blk �±�
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

};
*/

vector<record> blk2records(vector<int> vi);
#endif //RECORDMANAGER_RECORD_H
