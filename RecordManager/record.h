
#ifndef RECORDMANAGER_RECORD_H
#define RECORDMANAGER_RECORD_H

#include <iostream>
#include <map>
#include"../catalog/catalog.h"

using std::vector;
using std::string;

namespace RecordManager
{
	
// query <=> record <=> buffer <=> DB files
class record
{
	string tbname;
	bool isdeleted = false;
	unsigned int bytesize = 0;
	unsigned char* data = nullptr;
};



}


#endif //RECORDMANAGER_RECORD_H
