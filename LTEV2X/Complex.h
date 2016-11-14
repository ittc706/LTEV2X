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
	/*---构造函数---*/
	Complex();
	Complex(double t_Real);
	Complex(double t_Real, double t_Imag);
	Complex(const Complex& c);
	Complex(const std::initializer_list<double> il);


	/*---成员运算符重载---*/
	Complex& operator=(const Complex& c);
	Complex& operator+=(const Complex& c);
	Complex& operator-=(const Complex& c);


	/*---功能函数---*/
	Complex conjugate();//求共轭
	std::string toString();
	void print(std::ostream&out = std::cout);
};

Complex operator-(const Complex& c);
Complex operator+(const Complex& c1, const Complex& c2);
Complex operator-(const Complex& c1, const Complex& c2);
Complex operator*(const Complex& c1, const Complex& c2);
Complex operator/(const Complex& c1, const Complex& c2);
bool operator==(const Complex& c1, const Complex& c2);
bool operator!=(const Complex& c1, const Complex& c2);
