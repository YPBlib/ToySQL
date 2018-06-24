#include "record.h"

namespace recordspace
{
	vector<tbFreeList> freelist;
}

vector<int> loadtable(string tbname)
{
	// 装载表 
	auto vi = blockgen(tbname);
	// 更新catalog::tabelbase
	for (auto& i : catalog::tablebase)
	{
		if (i.tbname == tbname)
		{
			i.pages = vi;
			i.isinmemory = true;
		}
	}
	return vi;
}
//string tbfile = minisql::record_path + std::to_string(catalog::catamap[tbname]) + ".db";
// block to record
vector<shared_ptr<DataValue>> trans2record(vector<int> cstype,const int& bytes, int curch, unsigned char* ptr)
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
		if (d == tok_CHAR)
		{
			char tpc[256]{ 0 };
			for (int i = 0; i < bytes; ++i)
			{
				tpc[i] = (char)*(ptr + curch + i);
			}
			curch += bytes;
			result.push_back(std::move(std::make_shared<DataString>(tpc)));
		}
		if (d == tok_DOUBLE || d == tok_FLOAT)
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
void trans2block(unsigned char* dest, const vector<shared_ptr<DataValue>>& data, int bytes)
{
	int cnt = 0;
	for (auto i : data)
	{
		auto vc = i->emit_char();
		for (auto c : vc)
		{
			dest[cnt++] = (unsigned char)c;
		}
	}
}