#pragma once
#ifndef llvmsql_catalog
#define llvmsql_catalog
#include"../Interpreter/llvmsql.h"
#include"../env/envir.h"
using std::string;
using std::vector;
using std::shared_ptr;
// table (name,col_num,record_size,unic_key,index)
// col (name,type,prim,unic)
// index (name,which_table,which_col)

namespace catalog 
{

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
};

class SQLcol
{
	std::string name;
	int coltype;
	bool isprim = false;
	bool isunic = false;
	const unsigned char N = 0;
};



}

void make_cata(shared_ptr<CreateTableSimpleAST> T);

#endif
