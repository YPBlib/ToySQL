# catalog文件需要维护的信息
1. 当前表的数量
2. 表名到数值的map
2. 每个表的表名，字段数，record大小，每个字段的(name,type,[N],prim,unic,null)
# block 从硬盘读取
可能用到 app ate seekp seekg tellg 
# block 写回到硬盘

# block 转 queryable-record

# queryable-record 转 block

# 每个表名映射到1个数字(0000-FFFF)
# 映射表写到catalog文件里
# 每个文件存1k条记录，每个表对应多个文件，文件名为 tb0023_1, tbC9F4_4,...

# 表的建立
一个表最多可以定义32个属性，各属性可以指定是否为unique；支持单属性的主键定义。
# 表记录的删除
支持每次一条或多条记录的删除操作
# 表记录的查找
可以通过指定用and连接的多个条件进行查询，支持等值查询和区间查询。
# 数据类型
只要求支持三种基本数据类型：int，char(n)，float，其中char(n)满足 1 <= n <= 255
