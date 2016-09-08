#pragma once
#include<string>
#include<initializer_list>

class Complex {
public:
	static double abs(const Complex&c);

	/*���ݳ�Ա*/
	double real;
	double imag;

	/*���캯��*/
	Complex();
	Complex(double t_Real, double t_Imag);
	Complex(const Complex& c);
	Complex(const std::initializer_list<double> il);


	/*��Ա���������*/
	Complex& operator=(const Complex& c);
	Complex& operator+=(const Complex& c);
	Complex& operator-=(const Complex& c);


	/*���ܺ���*/
	Complex conjugate();//����
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
