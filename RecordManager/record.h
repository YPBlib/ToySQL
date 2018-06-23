
#ifndef RECORDMANAGER_RECORD_H
#define RECORDMANAGER_RECORD_H


#include"../catalog/catalog.h"
#include"../Buffer/buffer.h"

using std::vector;
using std::string;

namespace RecordManager
{

	vector<record> blk2record(const block& blk);
class recordtype
{
	virtual ~recordtype() = 0;
};

class recordint:public recordtype
{
public:
	int value = 0;
};
class recorddouble :public recordtype
{
public:
	double value = 0;
};
class recordstring :public recordtype
{
public:
	string value = 0;
};

class recordcolumn
{
public:
	string colname;
	shared_ptr<recordtype> value;
};

// query <=> record <=> buffer <=> DB files
class record
{
	string tbname;
	bool isdeleted = false;
	const unsigned int bytesize = 0;
	vector<const recordcolumn> data;
};

// trans raw binary data to queryable records
class queryable
{
	const string filename;
	const unsigned int offset;
	block& src;
	vector<record> data;
	queryable(block* blk) :filename(filename), offset(offset),src(*blk)
	{
		// trait tbname from filename

		// trait data from blk
		data = blk2record(src);
	}
};




}


#endif //RECORDMANAGER_RECORD_H
