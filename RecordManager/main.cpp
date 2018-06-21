#include <iostream>
#include <cstring>
#include <algorithm>
#include <bitset>
#include "record.h"

using namespace std;

bool judge(BaseData* data){
    int age = ((int*)data->getBytes())[0];
    return age>20;
}
void setBitMap(char * source, int size, int position);

int main() {

    char id[11] = "3160104339";
    char name[10] = "Jack Ma";
    int age = 20;
    vector<Column> attrs = {
            /* 依次是属性名，类型，大小，约束 */
            make_tuple("id", DB_STRING, 11, PRIMARY_KEY),
            make_tuple("name", DB_STRING, 10, NONE),
            make_tuple("age", DB_INT, 4, NONE)
    };

    Table *student = new Table(1, "student", attrs);

    vector<char *> record = {
            id,
            name,
            (char*)&age
    };
    student->insertRecord(record);

    return 0;

}
