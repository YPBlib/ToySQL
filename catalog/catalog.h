#pragma once
#ifndef llvmsql_catalog
#define llvmsql_catalog
#include"../Interpreter/llvmsql.h"

void loadcata();
void make_cata(shared_ptr<CreateTableSimpleAST> T);
inline int getbyte(int n);

namespace catalog 
{
	extern string cata_path;
	extern std::map<string, unsigned int> catamap;

class SQLcol
{
public:
	const string name;
	const unsigned int coltype;
	const unsigned char N = 0;
	bool isprim;
	bool isunic;
	bool isnull;
	SQLcol(const string& name,const unsigned int& coltype, const unsigned char& N, bool isprim, bool isunic, bool isnull) :
		name(name), coltype(coltype), N(N), isprim(isprim), isunic(isunic), isnull(isnull) {}
};

class SQLtable
{
public:
	const string name;
	const unsigned int col_num;
	const unsigned int record_size;
	vector<SQLcol> cols;
	vector<string> uniccols;
	vector<string> primcols;
	SQLtable(const string& name,const unsigned int& col_num,const unsigned int& record_size):
		name(name),col_num(col_num),record_size(record_size){}
};

}


#endif
