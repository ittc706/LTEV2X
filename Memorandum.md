* 内存还有泄漏
* 将RSU VeUE类内数据结构改成继承体系
* 将配置文件分为系统级以及各个单元
* 判断是否需要重新计算MCS的条件没有改
* 修改WT的实现
* 修改了TDM_DRA的干扰列表，但是逻辑没有检查
* 注意加入mencpy的头文件：#include<string.h>
* system头文件 #include <stdlib.h>z
* 线程数量作为参数
* 编码格式转为utf-8
* 头文件的问题
* 加载配置参数的类在Linux下有问题
* 修改日志文件路径