## 关于catalog
# catalog文件需要维护的信息
1. num.log 当前表的数量
2. map.log 表名到数值的map
3. #.log 每个表的表名，字段数，record大小(record+1Byte，表示是否已经删除)，每个字段的(name,type,[N],prim,unic,null)

# 初始化cata表信息应进行的动作
loadcata()
1. 读num.log 没有则创建
2. 读map.log 没有则创建 装载到catalog::catamap
3. 读各个#.log，装载到 catalog::tablebase

# 创建表对应进行的动作
0. 确认该表尚不存在
1. make_meta更新 catalog::catamap
2. make_meta更新 catalog::tablebase
3. 创建 #.log
4. 写回 map.log
5. 写回 num.log
6. 创建一个新的 #.db	(用"wb"打开关闭即可)

# 删除表对应进行的动作
0. 确认该表存在
1. 更新catamap
2. 清除 #.db
3. 清除 #.log
4. 写回 map.log
5. 写回 num.log


## 关于 buffer (1个表对应1个文件)
unsigned char** buff: 全局缓冲区
class block: 维护单个页的信息。并不存 binary 数据
vector<block<>> BufferManager: 维护buff中每个页的调度情况
vector<int> blockgen(const string& tbname): 把一整个表装载到buffer中，返回对应页的下标
std::pair<unsigned int,unsigned int> counttablebyte(const string& tbname): 获取该表大小，和该表单条record的大小
	,(从catalog获取，不要读文件) 即使不考虑异步读写文件，取内存也更快
void ReplacePage(int needy, bool(*f)(const block<>&,const block<>&)): 根据lambda做替换

# 初始化全局缓冲区对应的动作
initbuff()
1. 给buff分配内存
2. 初始化BufferManager

# block 从硬盘装载1整个表，通过blockgen实现
1. 计算需要的页数
2. 找出足够的页数，如果不够，写回脏页
3. 用可用的页装载文件，并返回下标记录 (blockgen 在parse掉1个StatementAST，并且miss后被调用)
可能用到 app ate seekp seekg tellg 
# block 写回到硬盘
block::writeback
1. "rb+"模式打开db文件
2. fseek跳到该block对应的起始位置
3. 用fwrite实现写回


## 关于record
维护每个表的数据是否在内存中，如果在的话范围是哪些(务必保证是连续范围,至少是个vector<int>)
namespace recordspace: 调度表中由于删除产生的空洞
# block 转 queryable-record
注意首字节是flag位
# queryable-record 转 block
注意首字节是flag位
## 关于exec
装载表用 blockgen

# 关于插入
tablerecord::insert()
0. 在consume ;后执行
1. 装载表,1个vector装载所有record
2. 将blk兑换成 vctor<record>
3. 插入前的检查: primary，unic
4. 更新catalog::tabelbase
5. 写入delete record位置，记录脏页
6. 如果不够，像blk尾部写入，记录脏页
7. 如果不够，开新页，记录脏页，更新catalog::tabelbase

# 关于select
1. 装载表,更新catalog::tabelbase
2. 确定写入文件
3. whereExpr == nullptr, 直接抽取字段，打印
4. whereExpr != nullptr, 对整个表的每条record过滤放入临时vector
5. 输出，fclose

# 关于删除
1. 装载表,更新catalog::tabelbase
2. 删除前的检查，先select(可能是1条或多条)
3. 删除，更新record.isdeleted, 更新脏页












# 表的建立
一个表最多可以定义32个属性，各属性可以指定是否为unique；支持单属性的主键定义。
# 表记录的删除
支持每次一条或多条记录的删除操作
# 表记录的查找
可以通过指定用and连接的多个条件进行查询，支持等值查询和区间查询。
# 数据类型
只要求支持三种基本数据类型：int，char(n)，float，其中char(n)满足 1 <= n <= 255
