#include "recorder.h"

namespace recordspace
{
	vector<tbFreeList> freelist;
}

vector<int> loadtable(string tbname)
{
	// BufferManager 所有脏页写回
	for (int i = 0; i != page_num; ++i)
	{
		if (BufferManager[i].isdirty && BufferManager[i].tbname == tbname)
			BufferManager[i].writeback();
	}
	// 装载表 
	auto vi = blockgen(tbname);
	// 更新catalog::tabelbase
	for (auto& i : catalog::tablebase)
	{
		if (i.tbname == tbname)
		{
			i.pages = vi;
		}
	}
	return vi;
}
//string tbfile = toysql::record_path + std::to_string(catalog::catamap[tbname]) + ".db";
// block to record
vector<shared_ptr<DataValue>> trans2record(vector<int> cstype,const int& bytes, int curch, char* ptr)
{
	vector<shared_ptr<DataValue>> result;
	for (auto d : cstype)
	{
		if (d == tok_INT)
		{
			int val = *reinterpret_cast<int*>(ptr + curch);
			curch += sizeof(int) / sizeof(unsigned char);
			result.push_back(std::move(std::make_shared<DataInt>(val)));
		}
		else if (d == tok_CHAR)
		{
			char tpc[256]{ 0 };
			for (int i = 0; i < bytes; ++i)
			{
				tpc[i] = (char)*(ptr + curch + i);
			}
			curch += bytes;
			result.push_back(std::move(std::make_shared<DataString>(tpc)));
		}
		else if (d == tok_DOUBLE || d == tok_FLOAT)
		{
			double val = *reinterpret_cast<double*>(ptr + curch);
			curch += sizeof(double) / sizeof(unsigned char);
			result.push_back(std::move(std::make_shared<DataDouble>(val)));
		}
		else
			throw runtime_error("wrong t in trans2record\n");
	}
	return result;
}

vector<record> blk2records(vector<int> vi)
{
	vector<record> result;
	for (auto i : vi)
	{
		block<> tmp_blk = BufferManager[i];
		// 获取各列类型的信息
		auto cs = catalog::tablebase[catalog::catamap[tmp_blk.tbname]].cols;
		vector<int> cstype;
		for (auto col : cs)
		{
			cstype.push_back(col.coltype);
		}
		int Size = tmp_blk.bytes;
		for (int j = 0; j < tmp_blk.recordnum; ++j)
		{
			result.push_back(record(i, j, Size, trans2record(cstype, tmp_blk.bytes, 0, buff[i])));
		}
	}
	return result;
}

// record to block
void trans2block(char* dest, const vector<shared_ptr<DataValue>>& data,bool isdirty)
{
	int cnt = 0;
	for (auto i : data)
	{
		auto vc = i->emit_char();
		for (auto c : vc)
		{
			dest[cnt++] = c;
		}
	}
}