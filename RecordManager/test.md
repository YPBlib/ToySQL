#### 定义类
- DynamicMemory: 其实就是包装了一下指针。
- Tuple: 一个元组数据包，可以序列化为二进制，也可以从二进制生成数据。（之后这个类可能重构）
- Attribute: 元组里面的一个属性，包括了值和一些信息。
- Page： 内存中的页面大小，对应于块大小4096B

#### 类测试
```
int main() {
    DynamicMemory memory(10);
    memory.put((void *)"abcdefghij", 10);
	
    memory.rewrite((void *)"1234", 4, 2);
    memory.rewrite((void *)"overflow", 8, 3);

    return 0;
}
//运行结果
put:
(gdb) x/10ab memory.getPtr()
0x60004c4e0:	0x61	0x62	0x63	0x64	0x65	0x66	0x67	0x68
0x60004c4e8:	0x69	0x6a
rewrite:
(gdb) x/10ab memory.getPtr()
0x60004c4e0:	0x61	0x62	0x31	0x32	0x33	0x34	0x67	0x68
0x60004c4e8:	0x69	0x6a
rewrite(overflow):
(gdb) x/10ab memory.getPtr()
0x60004c4e0:	0x61	0x62	0x31	0x32	0x33	0x34	0x67	0x68
0x60004c4e8:	0x69	0x6a
```
```
int main() {
    Tuple previous(1, "table");
    //准备数据
    int age = 18;
    float gpa = 5.04;
    char name[10] = "XijunLiao";
    //产生属性
    Attribute attr_1(&age, sizeof(int), DB_INT);
    Attribute attr_2(name, 40, DB_STRING);
    Attribute attr_3(&gpa, sizeof(float), DB_FLOAT);

    previous.put(string("age"), attr_1);
    previous.put(string("name"), attr_2);
    previous.put(string("gpa"), attr_3);

    DynamicMemory memory = previous.serialize();

    Tuple now(1, "table");
    //产生属性模板
    Attribute attr_4(nullptr, sizeof(int), DB_INT);
    Attribute attr_5(nullptr, 40, DB_STRING);
    Attribute attr_6(nullptr, sizeof(float), DB_FLOAT);

    now.put(string("age"), attr_4);
    now.put(string("name"), attr_5);
    now.put(string("gpa"), attr_6);

    now.loadData(memory);

    return 0;
}

//运行结果：Tuple now是previous的一个copy。
```