#pragma once
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include "Exception.h"

class Complex;//Complex类前置声明（由于下列4个函数的定义与Complex的定义相互包含）
double g_abs2(const Complex& a);
double g_abs(const Complex& a);
Complex g_Conjugate(const Complex& a);
double g_Factorial(int K);

class Complex {
public:
	/*数据成员*/
	double m_Real;
	double m_Imag;

	/*构造函数*/
	Complex();
	Complex(double r);
	Complex(double r, double i);
	Complex(const Complex& a);//拷贝构造函数
	~Complex();//析构函数

	/*成员运算符重载*/
	Complex& operator =(const double& a);
	Complex& operator =(const Complex& c);
	Complex& operator +=(const Complex& c);
	Complex& operator -=(const Complex& c);
};
/*以下算数运算符不作为Comlex的成员函数*/
Complex operator +(const Complex& c1, const Complex& c2);
Complex operator -(const Complex& c1, const Complex& c2);
Complex operator *(const Complex& c1, const Complex& c2);
Complex operator *(const Complex& c1, const double& a);
Complex operator /(const Complex& c1, const Complex& c2);
Complex operator /(const Complex& c1, const double& a);





class Matrix {
	static Complex s_A[64];
	static int s_pi[64];
	static Complex s_e[8];
	static Complex s_y[8];
	static int s_parity;
public:
	/*数据成员*/
	int m_Rows;//行数
	int m_Cols;//列数
	Complex *m_Mem;//动态指针
	
	/*构造函数*/
private:
	void copy(const Matrix& a);
public:
	void Matrix::initialize(int r, int c);//被构造函数调用的初始化函数
	Matrix();
	Matrix(int r, int c);
	Matrix(const Matrix& m);//拷贝构造函数，由于存在动态分配的指针，需要自定义拷贝部分
	
	/*资源管理函数*/
	void clean();//释放资源
	~Matrix();//析构函数

	/*矩阵功能函数*/
	void clear();//置零矩阵所有元素
	Complex get(int row, int col);//返回矩阵指定位置的元素
	void set(Complex a, int row, int col);//矩阵指定位置元素赋值
	Matrix diag();//获取矩阵对角线元素
	void eye();//把矩阵变为单位阵
	Matrix trans();//得到矩阵转置
	Matrix hermitian();//得到矩阵共轭转置
	Complex det();//计算行列式
	Complex tr();//计算矩阵的迹
	Matrix inv();//得到矩阵的逆矩阵???
	Matrix inverse();//矩阵求逆
	double norm_2();//求矩阵二范数
	void svd(Matrix* u, Matrix* s, Matrix* v);//SVD分解
	void appendInRow(const Matrix& a);//矩阵行扩容
	void appendInCol(const Matrix& a);//矩阵列扩容
	Matrix apart(int rs, int re, int cs, int ce);//???
	int _LUP_Decomposition(Complex* A, int n, int* pi);//???
	void _LUP_Solve(Complex* LU, int* pi, Complex *b, Complex* out, int n);//???
	

	/*成员函数运算符重载*/
	Matrix& operator =(const Matrix& m);
	Matrix& operator +=(const Matrix& m);
	Matrix& operator *=(const double& a);
	Matrix& operator /=(double a);
};
/*以下算数运算符不作为Comlex的成员函数*/
Matrix operator + (const Matrix& m1,const Matrix& m2);
Matrix operator - (const Matrix& m1, const Matrix& m2);
Matrix operator *(const Matrix& m1, double a);
Matrix operator *(const Matrix& m1, const Matrix& m2);
Matrix operator /(const Matrix& m1, const double& a);
Matrix operator /(const Matrix& m1, const Complex& a);





