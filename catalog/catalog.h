#pragma once
#ifndef toysql_catalog
#define toysql_catalog
#include "../Interpreter/common.h"

void loadcata();
void make_cata(shared_ptr<CreateTableSimpleAST> T);
int getbyte(int n);

namespace catalog 
{
	extern std::string cata_path;
	extern std::map<string, unsigned int> catamap;
	

class SQLcol
{
public:
	string colname;
	int coltype;
	unsigned int N = 0;
	bool isprim;
	bool isunic;
	bool isnull;
	SQLcol(const string& colname,const unsigned int& coltype, const unsigned char& N, bool isprim, bool isunic, bool isnull) :
		colname(colname), coltype(coltype), N(N), isprim(isprim), isunic(isunic), isnull(isnull) {}
};

class SQLtable
{
public:
	
	string tbname;
	unsigned int col_num;
	unsigned int record_size;
	vector<SQLcol> cols;
	vector<string> uniccols;
	vector<string> primcols;

	
	bool isinmemory = false;
	int record_num = 0;
	vector<int> pages;
	int beginpos;	
	int endpos;	
	
	SQLtable() = default;
	SQLtable(const string& tbname,const unsigned int& col_num,const unsigned int& record_size):
		tbname(tbname),col_num(col_num),record_size(record_size){}
};
extern vector<SQLtable> tablebase;
}


#endif
