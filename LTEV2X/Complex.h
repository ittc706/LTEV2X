#pragma once
#include<string>
#include<initializer_list>

class Complex {
	/*------------------静态------------------*/
public:
	static double abs(const Complex&c);

	/*------------------域------------------*/
public:
	double real;
	double imag;

	/*------------------方法------------------*/
	/*
	* 默认构造函数
	* 初始化为(0,0)
	*/
	Complex();

	/*
	* 接受一个实参的构造函数
	* 该double类型的作为实部，虚部为0
	* 接受该类型的隐式转换
	*/
	Complex(double t_Real);

	/*
	* 接受两个实参的构造函数
	* 分别为实部和虚部
	*/
	Complex(double t_Real, double t_Imag);

	/*
	* 拷贝构造函数
	*/
	Complex(const Complex& c);

	/*
	* 列表初始化
	* 列表长度可以是0,1,2,
	*		0的时候，初始化为(0,0)
	*		1的时候，用于初始化实部
	*		2的时候，用于初始化实部和虚部
	*/
	Complex(const std::initializer_list<double> il);


	/*
	* 拷贝赋值运算符
	*/
	Complex& operator=(const Complex& c);

	/*
	* 复合赋值运算符
	*/
	Complex& operator+=(const Complex& c);

	/*
	* 复合赋值运算符
	*/
	Complex& operator-=(const Complex& c);


	/*
	* 求共轭
	*/
	Complex conjugate();

	/*
	* 生成格式化字符串
	*/
	std::string toString();

	/*
	* 用于打印到指定输出流
	* 默认的输出流为std::out
	*/
	void print(std::ostream&out = std::cout);
};


Complex operator-(const Complex& c);
Complex operator+(const Complex& c1, const Complex& c2);
Complex operator-(const Complex& c1, const Complex& c2);
Complex operator*(const Complex& c1, const Complex& c2);
Complex operator/(const Complex& c1, const Complex& c2);
bool operator==(const Complex& c1, const Complex& c2);
bool operator!=(const Complex& c1, const Complex& c2);
