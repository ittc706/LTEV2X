#pragma once

/*
* 前置声明
*/
class GTT_Road;

/*
* 每一个单元都有该单元对应的Road视图
* 该类的作用就是提供不同单元之间Road实体的信息交互
* 目前仅有GTT单元需要有Road
*/

class Road{
	friend class System;
	/*------------------域------------------*/
private:
	/*
	* 指向GTT视图下的Road实体对象
	*/
	GTT_Road * m_GTT = nullptr;

	/*------------------方法------------------*/
public:
	/*
	* 返回指向GTT视图下的Road的指针
	*/
	GTT_Road* getGTTPoint() { return m_GTT; }
};