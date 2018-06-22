#pragma once
#ifndef llvmsql_catalog
#define llvmsql_catalog
#include"../Interpreter/llvmsql.h"
#include"../env/envir.h"
using std::string;
using std::vector;
using std::shared_ptr;


namespace catalog 
{
	extern string cata_path;
class SQLcol;
class SQLtable
{
public:
	std::string name;
	int col_num;
	const unsigned int record_size;
	vector<SQLcol> cols;
	vector<string> uniccols;
	vector<string> primcols;
	SQLtable(const unsigned int record_size):record_size(record_size){}
};

class SQLcol
{
public:
	std::string name;
	int coltype;
	bool isprim = false;
	bool isunic = false;
	bool isnull = false;
	const unsigned char N = 0;
	SQLcol(string name,int coltype,const unsigned char N,bool isprim ,bool isunic ,bool isnull ):
		name(name),coltype(coltype),N(N),isprim(isprim),isunic(isunic),isnull(isnull){}
};

}

void make_cata(shared_ptr<CreateTableSimpleAST> T);
inline int getbyte(int n);





#endif
