与测试仪器SCPI通讯并解析数据

数据保存入sqlite数据库, 并使用sqlite-orm库作orm工具CURD

亮点主要是自定义的QAbstractTableModel 中对于sturct类型数据的更新
通过boost::pfr可以用整形字面量访问struct某个成员。
使用std::integer_sequence编译期数组(c++14)
通过将转化函数指针放入编译期数组建立映射关系，可以使struct成员和QVariant相互转化,从而无需写switch(index.colum()){}
具体看qvariant_cvt.hpp