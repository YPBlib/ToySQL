#include"catalog.h"
using minisql::minisql_path;
using std::endl;

/*
#include <boost/serialization/serialization.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
*/

namespace catalog
{
	string cata_path;
	std::map<string, unsigned int> catamap;
	static unsigned int tablenum=0;
}

void init_cata()
{
	catalog::cata_path = minisql_path + "catalog\\";
	loadcata();
}

inline int getbyte(int n)
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

void make_cata(shared_ptr<CreateTableSimpleAST> T)
{
	int prim_flag = 0;
	string tbname = *T->table_name->id.get();
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
			throw std::runtime_error("col_name,prim,uni,forei,check of a createdefAST are all nullptr,cannot parse\n");
		}
	}
	// attach primary attr to cols
	for (auto j : primcols)
	{
		for (auto& k : cols)
		{
			if (j == k.name)
			{
				k.isprim = true;
			}
		}
	}
	// update catamap
	catalog::tablenum++;
	catalog::catamap.insert({ tbname, catalog::tablenum });
	// write to map.log   // no need to update, catamap is updated when quiting
	/*
	string mAp = catalog::cata_path + "map.log";
	ofstream fs(mAp, ofstream::app );
	fs << tbname << "  " << cols.size() << std::endl;
	fs.close();
	*/
	// write to tbname.log
	unsigned int un = catalog::catamap[tbname];
	string cata_file = catalog::cata_path +std::to_string(un) + ".log";	
	std::ofstream w(cata_file);
	w << tbname << std::endl;
	w << col_num << std::endl;
	int bytes = 0;
	for (auto j : cols)
	{
		bytes += getbyte(j.coltype)*j.N;
	}
	w << bytes << std::endl;
	for (auto i : cols)
	{
		w << i.name << "  " << i.coltype << "  " << (int)i.N
			<< "  " << (i.isprim ? 1 : 0) << "  " << (i.isunic ? 1 : 0) << "  " << (i.isnull ? 1 : 0) << std::endl;
	}
	w.close();
}

void loadcata()
{
	string num = catalog::cata_path + "num.log";
	FILE* r = fopen(num.c_str(), "r");
	if (r == nullptr)
	{
		ofstream ofs(num);
		ofs << 0;
		ofs.close();
	}
	ifstream ifs(num);
	ifs >> catalog::tablenum;
	ifs.close(); 
	
	string sss;
	unsigned int ui;
	string mAp = catalog::cata_path + "map.log";
	if (r=fopen(mAp.c_str(), "r"))
	{
		fclose(r);
		std::ifstream mAP(mAp);
		for (unsigned int i = 0u; i != catalog::tablenum; ++i)
		{
			mAP >> sss >> ui;
			catalog::catamap.insert({ sss, ui });
		}
		mAP.close();
	}
	
	
	
	if (catalog::catamap.size() != catalog::tablenum)
		throw runtime_error("catalog::catamap.size()!=catalog::tablenum\n");

}

void cata_wb()
{
	// update num.log
	string num = catalog::cata_path + "num.log";
	ofstream wn(num);
	wn << catalog::tablenum << endl;
	wn.close();
	// update map.log
	string mAp = catalog::cata_path + "map.log";
	ofstream ofs(mAp);
	for (auto i : catalog::catamap)
	{
		ofs << i.first << "  " << i.second << std::endl;
	}
}