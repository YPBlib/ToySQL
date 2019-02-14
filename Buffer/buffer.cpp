#include"buffer.h"

char** buff;

vector<block<>> BufferManager;


void initbuff()
{
	buff = new char*[page_num];
	for (int i = 0; i < page_num; ++i)
	{
		buff[i] = new char[BLOCK_8k];
		BufferManager.push_back(block<>(i, false));
	}
}

void ReplacePage(int needy, bool(*f)(const block<>&,const block<>&))
{
	std::sort(BufferManager.begin(), BufferManager.end(), f);
	int cnt = 0;
	for (auto i = BufferManager.begin(); i != BufferManager.end(); ++i)
	{
		if (cnt != needy && !i->ispin)
		{
			i->writeback();
			++cnt;
		}
	}
}


std::pair<int,int> counttablebyte(const string& tbname)
{
	string tb_log= catalog::cata_path + std::to_string(catalog::catamap[tbname]) + ".log";
	unsigned int ui;string temp; unsigned int record_size;
	ifstream ifs2(tb_log);
	ifs2 >> temp >> ui >> record_size;
	////
	int tb_index = catalog::catamap[tbname];
	int record_num = catalog::tablebase[tb_index].record_num;
	return std::make_pair(record_num*record_size, record_size);
}

// ����һ���������ļ�д��block  // ע����λ
vector<int> blockgen(const string& tbname)
{
	auto tbsizeinfo = counttablebyte(tbname);
	unsigned int recordnum = tbsizeinfo.first / tbsizeinfo.second;
	unsigned int records_pre_blk = BLOCK_8k / tbsizeinfo.second;
	unsigned int blknum = recordnum / records_pre_blk + (recordnum > recordnum / records_pre_blk*records_pre_blk) ? 1u : 0u;
	if (blknum > page_num)
	{
		throw runtime_error("Error: <del>the table `"+tbname+"` is too large</del>\n");
	}
	vector<unsigned int>tempi(page_num);
	std::transform(BufferManager.cbegin(), BufferManager.cend(), tempi.begin(),
		[](const block<>& blk)->unsigned int {return (blk.isdirty||blk.ispin) ? 0 : 1; });
	auto sum = std::accumulate(tempi.cbegin(), tempi.cend(), 0);
	if (sum < blknum)
	{
		ReplacePage(blknum - sum, [](const block<>& a, const block<>& b) {return a.getfreq() < b.getfreq(); });
	}
	// �ָ�ԭ˳��
	std::sort(BufferManager.begin(), BufferManager.end(),
		[](const block<>& a, const block<>& b) {return a.series < b.series; });
	vector<int> result;
	for (auto& i : BufferManager)
	{
		if ((!(i.isdirty||i.ispin)) && result.size() != blknum)
			result.push_back(i.series);
	}
	string tb_db = toysql::record_path + std::to_string(catalog::catamap[tbname]) + ".db";
	FILE* r = fopen(tb_db.c_str(), "rb");
	
	int dist = 0;
	for (auto i : result)
	{
		BufferManager[i].tbname = tbname;
		BufferManager[i].filename = tb_db; 
		if (recordnum > records_pre_blk)
			BufferManager[i].recordnum = records_pre_blk;
		else BufferManager[i].recordnum = recordnum;
		BufferManager[i].bytes = tbsizeinfo.second;
		BufferManager[i].offset = dist;
		dist += BufferManager[i].recordnum*BufferManager[i].bytes;
		recordnum -= records_pre_blk;
		BufferManager[i].updatefreq();
		// ��һ���ܿ��ܲ���8k
		fread(buff[i], sizeof(unsigned char), dist, r);
	}
	fclose(r);
	return result;
}
