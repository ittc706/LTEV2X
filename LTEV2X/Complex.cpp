/*
* =====================================================================================
*
*       Filename:  Complex.cpp
*
*    Description:  Complex实现
*
*        Version:  1.0
*        Created:
*       Revision:
*       Compiler:  VS_2015
*
*         Author:  HCF
*            LIB:  ITTC
*
* =====================================================================================
*/

#include<cmath>
#include<sstream>
#include<iomanip>
#include<iostream>
#include"Complex.h"
#include"Exception.h"

using namespace std;

double Complex::abs(const Complex&c) {
	double res = 0;
	res += sqrt(c.real*c.real + c.imag*c.imag);
	return res;
}


Complex::Complex() :
	real(0), imag(0) {}


Complex::Complex(double t_Real) :
	real(t_Real), imag(0) {}


Complex::Complex(double t_Real, double t_Imag) :
	real(t_Real), imag(t_Imag) {}


Complex::Complex(const Complex& c) :
	real(c.real), imag(c.imag) {}


Complex::Complex(const std::initializer_list<double> il) {
	if (il.size() > 2) throw Exp("Complex初始化列表元素数目错误：应该为两个double类型的元素");
	initializer_list<double>::iterator it = il.begin();
	switch (il.size()) {
	case 0:
		real = 0;
		imag = 0;
		break;
	case 1:
		real = *it;
		imag = 0;
		break;
	case 2:
		real = *it++;
		imag = *it;
	}
}


Complex& Complex::operator =(const Complex& c) {
	this->real = c.real;
	this->imag = c.imag;
	return *this;
}
Complex& Complex::operator +=(const Complex& c) {
	this->real += c.real;
	this->imag += c.imag;
	return *this;
}
Complex& Complex::operator -=(const Complex& c) {
	this->real -= c.real;
	this->imag -= c.imag;
	return *this;
}


Complex Complex::conjugate() {
	Complex res;
	res.real = real;
	res.imag = -imag;
	return res;
}


string Complex::toString() {
	ostringstream ss;
	ss.precision(2);
	//ss << fixed;//避免用科学计数法，因为可能会存在极小的数值
	ss << "[" << left << setw(2) << real <<"+ j"<< left << setw(2) << imag << "]";
	return ss.str();
}

void Complex::print(std::ostream&out) {
	out << toString() << endl;
}


Complex operator-(const Complex& c) {
	Complex res;
	res.real = -c.real;
	res.imag = -c.imag;
	return res;
}

Complex operator+(const Complex& c1, const Complex& c2) {
	return Complex(c1.real + c2.real, c1.imag + c2.imag);
}


Complex operator-(const Complex& c1, const Complex& c2) {
	return Complex(c1.real - c2.real, c1.imag - c2.imag);
}


Complex operator*(const Complex& c1, const Complex& c2) {
	double realPart = 0;
	double imagPart = 0;
	realPart = c1.real*c2.real - c1.imag*c2.imag;
	imagPart = c1.real*c2.imag + c2.real*c1.imag;
	return Complex(realPart, imagPart);
}


Complex operator/(const Complex& c1, const Complex& c2) {
	double tmp = Complex::abs(c2);
	double denominator = tmp*tmp;//分母
	if (denominator == 0) throw Exp("Complex.cpp->Complex operator/(const Complex& c1, const Complex& c2)");

	double realPart = 0;
	double imagPart = 0;
	realPart = (c1.real*c2.real + c1.imag*c2.imag) / denominator;
	imagPart = (-c1.real*c2.imag + c2.real*c1.imag) / denominator;
	return Complex(realPart, imagPart);
}


bool operator==(const Complex& c1, const Complex& c2) {
	return c1.real == c2.real&&c1.imag == c2.imag;
}


bool operator!=(const Complex& c1, const Complex& c2) {
	return !(c1 == c2);
}