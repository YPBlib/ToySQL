#include "catalog.h"
#include <iostream>
using std::endl;


namespace catalog
{
	class SQLtable;
	std::string cata_path;
	std::map<std::string, unsigned int> catamap;
	std::vector<SQLtable> tablebase;
}

void init_cata()
{
	string path;
	catalog::cata_path = path + "catalog/";
	loadcata();
}

int getbyte(int n)
{
	if (n == tok_INT)
		return sizeof(int);
	if (n == tok_CHAR)
		return 1;
	if (n == tok_DOUBLE || n == tok_FLOAT)
		return sizeof(double);
	else
	{
		throw std::runtime_error("wrong arg in getbyte\n");
	}
}

void make_cata(std::shared_ptr<CreateTableSimpleAST> T)
{
	int prim_flag = 0;
	string tbname = *T->table_name->id.get();

	if (catalog::catamap.find(tbname) != catalog::catamap.end())
	{
		std::cout << "warning: the table `" << tbname << "` already exists. \n" << endl;
		return;
	}
	auto defs = T->create_defs;
	vector<catalog::SQLcol> cols;
	vector<string> uniccols;
	vector<string> primcols;
	int col_num = 0;
	for (auto i : defs)
	{
		if (i->colname)
		{
			col_num++;
			string colname = *i->colname->id.get();
			int dtype = i->coldef->dtype->dtype;
			const unsigned char N = i->coldef->dtype->n;
			bool isnull = i->coldef->null_flag;
			bool isunic = i->coldef->unic_flag;
			// attach unic attr to cols
			if (isunic)uniccols.push_back(colname);
			bool isprim = false;
			bool temp_isprim = i->coldef->primary_flag;
			if (temp_isprim)
			{
				if (prim_flag == 0)
				{
					isprim = true;
					prim_flag++;
				}
				else
				{
					throw std::runtime_error("multiple primary key defined\n");
				}
			}
			cols.push_back(std::move(catalog::SQLcol(colname, dtype, N, isprim, isunic, isnull)));
		}
		else if (i->prim)
		{
			if (prim_flag)
			{
				throw std::runtime_error("multiple primary key defined\n");
			}
			else if (i->prim->cols.size() > 1)
			{
				throw std::runtime_error("compound primary key is not supported for now\n");
			}
			else
			{
				for (auto j : i->prim->cols)
				{
					primcols.push_back(*j->id.get());
				}
				prim_flag++;
			}
		}
		else if (i->uni)
		{
			;
		}
		else if (i->forei)
		{
			;
		}
		else if (i->check)
		{
			;
		}
		else
		{
			throw std::runtime_error("col_name,prim,unic,forei,check of a createdefAST are all nullptr,cannot parse\n");
		}
	}
	// attach primary attr to cols
	for (auto j : primcols)
	{
		for (auto& k : cols)
		{
			if (j == k.colname)
			{
				k.isprim = true;
			}
		}
	}
	
	int bytes = 0;
	for (auto j : cols)
	{
		bytes += getbyte(j.coltype)*(int)j.N;
	}
	if (bytes > BLOCK_8k)
		throw runtime_error("Error one single record > 8K\n");
	// update catamap
	int x = catalog::catamap.size() ;
	catalog::catamap.insert({ tbname, x });
	// update tablebase
	catalog::SQLtable sqltb = catalog::SQLtable(tbname, col_num, bytes);
	sqltb.cols = cols;
	sqltb.primcols = primcols;
	sqltb.uniccols = uniccols;
	catalog::tablebase.push_back(sqltb);
	//  #.log
	int un = catalog::catamap[tbname];
	std::string cata_file = catalog::cata_path +std::to_string(un) + ".log";	
	std::ofstream w(cata_file);
	w << tbname << std::endl;
	w << col_num << std::endl;
	w << bytes << std::endl;
	for (auto i : cols)
	{
		int strange_bug_n = (int)i.N;
		w << i.colname << "  " << i.coltype << "  " << (strange_bug_n)
			<< "  " << (i.isprim ? 1 : 0) << "  " << (i.isunic ? 1 : 0) << "  " << (i.isnull ? 1 : 0) << std::endl;
	}
	w.close();
	// map.log
	string maplog = catalog::cata_path + "map.log";
	ofstream fsmap(maplog, ofstream::app );
	fsmap << tbname << "  " << catalog::catamap.size() << std::endl;
	fsmap.close();
	// num.log
	string numlog = catalog::cata_path + "num.log";
	ofstream fsnum(numlog);
	fsnum << catalog::catamap.size() << endl;
	fsnum.close();
	// #.db	"wb"
	string dbfile=toysql::record_path+ std::to_string(un) + ".db";
	FILE* www = fopen(dbfile.c_str(), "wb");
	fclose(www);
}

void loadcata()
{
	// load num
	string num = catalog::cata_path + "num.log";

	FILE* r = fopen(num.c_str(), "r");
	if (r == nullptr)
	{
		ofstream ofs(num);
		ofs << 0;
		ofs.close();
	}
	int x = 0;
	ifstream ifs(num);
	ifs >> x;
	ifs.close(); 
	// load map
	string sss;
	unsigned int ui;
	string mAp = catalog::cata_path + "map.log";
	// 
	if (r=fopen(mAp.c_str(), "r"))
	{
		fclose(r);
		std::ifstream mAP(mAp);
		for (unsigned int i = 0u; i != x; ++i)
		{
			mAP >> sss >> ui;
			catalog::catamap.insert({ sss, ui });
		}
		mAP.close();
	}
	if (catalog::catamap.size() != x)
		throw runtime_error("catalog::catamap.size()!=catalog::tablenum\n");
	//load tablebase
	for (auto i : catalog::catamap)
	{
		string tblog = catalog::cata_path + std::to_string(i.second) + ".log";
		ifstream ifs(tblog);
		string tbname; int colnum; int bytes; string colname; int ty; int N; int Isprim, Isunic, Isnull;
		ifs >> tbname >> colnum >> bytes;
		auto temptb = catalog::SQLtable(tbname, colnum, bytes);
		for (int j = 0; j < colnum; ++j)
		{
			ifs >> colname >> ty >> N >> Isprim >> Isunic >> Isnull;
			temptb.cols.push_back(catalog::SQLcol(colname, ty, N, Isprim, Isunic, Isnull));
			if (Isprim)
				temptb.primcols.push_back(colname);
			if (Isunic)
				temptb.uniccols.push_back(colname);
		}
		catalog::tablebase.push_back(temptb);
		ifs.close();
	}
}

void cata_wb()
{
	// update num.log
	string num = catalog::cata_path + "num.log";
	ofstream wn(num);
	wn << catalog::catamap.size() << endl;
	wn.close();
	// update map.log
	string mAp = catalog::cata_path + "map.log";
	ofstream ofs(mAp);
	for (auto i : catalog::catamap)
	{
		ofs << i.first << "  " << i.second << std::endl;
	}
}