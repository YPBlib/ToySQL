#include "record.h"

class DataValue
{
public:
	virtual const void* getValue() = 0;
	virtual ~DataValue() = default;
};

class DataInt :public DataValue
{
public:
	int value = 0;
	const void *getValue() { return (int*)&value; }
	~DataInt() = default;
};

class DataDouble :public DataValue
{
public:
	double value = 0;
	const void *getValue() { return (double*)&value; }
	~DataDouble() = default;

};

class DataString :public DataValue
{
public:
	string value;
	const void *getValue() { return (string*)&value; }
	~DataString() = default;
};

class record
{
	using seg = std::pair<unsigned int, unsigned int>;
public:
	//  blk的坐标
	unsigned int pos = 0u;
	//  在blk上的范围
	seg range;
	const tablerecord& tb;
	vector<shared_ptr<DataValue>> data;
};

// block to record
class tablerecord
{
	using info = std::pair<int, string>;
	//using seg = std::tuple<unsigned int, unsigned int, unsigned int,unsigned int>;
	using seg = std::pair<std::pair<unsigned int, unsigned int>,std::pair< unsigned int, unsigned int>>;
public:
	catalog::SQLtable tbinfo;
	bool isdeleted = false;
	//  表中所有数据的位置加和
	vector<seg> range;
	vector<record> data;
	void initdata()
	{
		;
	}

	void computerange()
	{
		;
	}

	void i_want_record()
	{
		
	}
	// 先清零
	void back_to_block()
	{

	}

	info insert()
	{
		return info(0, "");
	}
	info select()
	{
		return info(0, "");
	}
	info dele()
	{
		return info(0, "");
	}
};

