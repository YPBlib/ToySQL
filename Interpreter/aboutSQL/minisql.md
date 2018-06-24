## 关于catalog
# catalog文件需要维护的信息
1. num.log 当前表的数量
2. map.log 表名到数值的map
3. #.log 每个表的表名，字段数，record大小，每个字段的(name,type,[N],prim,unic,null)

# 初始化cata表信息应进行的动作
loadcata()
1. 读num.log 没有则创建
2. 读map.log 没有则创建 装载到catalog::catamap
3. 读各个#.log，装载到 catalog::tablebase

# 创建表对应进行的动作
1. 更新 catalog::catamap, catalog::tablebase
2. 创建 #.log
3. 写回 map.log
4. 写回 num.log

# 删除表对应进行的动作
0. 确认该表存在
1. 更新catamap
2. 清除 #.db
3. 清除 #.log
4. 写回 map.log
5. 写回 num.log


## 关于 buffer
# buff unsigned char**, 是全局缓冲区
# class block: 维护单个页的信息。并不存 binary 数据
# vector<block<>> BufferManager: 维护buff中每个页的调度情况
# vector<int> blockgen(const string& tbname): 把一整个表装载到buffer中，返回对应页的下标
# std::pair<unsigned int,unsigned int> counttablebyte(const string& tbname): 获取该表大小，和该表单条record的大小
	(从catalog获取，不要读文件) 即使不考虑异步读写文件，取内存也更快
# void ReplacePage(int needy, bool(*f)(const block<>&,const block<>&)): 根据lambda做替换
# 初始化全局缓冲区对应的动作
initbuff()
# block 从硬盘读取
可能用到 app ate seekp seekg tellg 
# block 写回到硬盘



## 关于record
# block 转 queryable-record

# queryable-record 转 block














# 表的建立
一个表最多可以定义32个属性，各属性可以指定是否为unique；支持单属性的主键定义。
# 表记录的删除
支持每次一条或多条记录的删除操作
# 表记录的查找
可以通过指定用and连接的多个条件进行查询，支持等值查询和区间查询。
# 数据类型
只要求支持三种基本数据类型：int，char(n)，float，其中char(n)满足 1 <= n <= 255
