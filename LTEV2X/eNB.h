#pragma once

/*
* 前置声明
*/
class GTT_eNB;

/*
* 每一个单元都有该单元对应的eNB视图
* 该类的作用就是提供不同单元之间eNB实体的信息交互
* 目前仅有GTT单元需要有eNB
*/

class eNB{
	friend class System;
	/*------------------域------------------*/
private:
	/*
	* 指向GTT视图下的eNB实体对象
	*/
	GTT_eNB * m_GTT = nullptr;

	/*------------------方法------------------*/
public:
	/*
	* 指向GTT视图下的eNB的指针
	*/
	GTT_eNB* getGTTPoint() { return m_GTT; }
};
