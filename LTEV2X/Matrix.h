#pragma once
#include<vector>
#include<random>
#include<string>
#include<utility>
#include<iostream>
#include"Complex.h"
#include"Exception.h"

/*
* 定义此类的目的是为了在越界访问的时候抛出异常，而非产生一个运行时错误，更利于错误的定位
* vector下标的越界访问
*/
class RowVector {
	/*数据成员*/
	std::vector<Complex> rowVector;//底层容器，设为私有禁止直接访问
public:
	/*数据成员*/
	int col;//向量长度，由于是行向量，这里称为列数

	/*构造函数*/
	RowVector();
	explicit RowVector(int t_Col);
	RowVector(const RowVector& t_RowVector);
	RowVector(const std::initializer_list<Complex> il);

	/*成员运算符重载*/
	RowVector& operator=(const RowVector& t_RowVector);
	Complex& operator[](int pos);
	const Complex& operator[](int pos) const;

	/*其他功能函数*/
	void resize(int size);
	std::string toString();
	void print(std::ostream&out = std::cout);
};
//行向量单目取反运算符
RowVector operator-(const RowVector& t_RowVector);

//行向量与行向量的运算
RowVector operator+(const RowVector& t_RowVector1, const RowVector& t_RowVector2);

//行向量与复数的运算
RowVector operator+(const RowVector& t_RowVector, const Complex&t_Complex);
RowVector operator+(const Complex&t_Complex, const RowVector& t_RowVector);

RowVector operator-(const RowVector& t_RowVector, const Complex&t_Complex);
RowVector operator-(const Complex&t_Complex, const RowVector& t_RowVector);

RowVector operator*(const RowVector& t_RowVector, const Complex&t_Complex);
RowVector operator*(const Complex&t_Complex, const RowVector& t_RowVector);

RowVector operator/(const RowVector& t_RowVector, const Complex&t_Complex);
RowVector operator/(const Complex&t_Complex, const RowVector& t_RowVector);

RowVector elementProduct(const RowVector& t_RowVector1, const RowVector& t_RowVector2);
RowVector elementDivide(const RowVector& t_RowVector1, const RowVector& t_RowVector2);


class Matrix {
	/*数据成员*/
	std::vector<RowVector> matrix;//底层多维容器，设为私有禁止直接访问
public:
	/*静态数据成员*/
	static std::default_random_engine s_Engine;

	/*数据成员*/
	int row;//行数
	int col;//列数


	/*构造函数*/
	Matrix();
	Matrix(int t_Row, int t_Col);
	Matrix(const Matrix& t_Matrix);
	Matrix(const std::initializer_list<RowVector> il);
	

	/*矩阵功能函数*/
	void randomFill(double realLeft, double readRight, double imagLeft, double imagRight);
	Matrix conjugate();//求共轭
	Matrix transpose();//求转置
	Matrix hermitian();//共轭转置
	Matrix inverse();//求逆矩阵
	Matrix diag();//求对角线构成的矩阵(行向量)


	/*成员运算符重载*/
	Matrix& operator=(const Matrix& t_Matrix);//赋值运算符
	RowVector& operator[](int pos);//下标运算符(非常量版本)
	const RowVector& operator[](int pos) const;//下标运算符(常量版本)


	/*其他功能函数*/
	std::string toString();
	void print(std::ostream&out = std::cout);


	/*用于构造对象的静态函数*/
	static Matrix buildDdentityMatrix(int t_Row);

	/*实现函数*/
	static Matrix merge(const Matrix& t_Matrix1, const Matrix& t_Matrix2);
	static std::pair<Matrix, Matrix> split(const Matrix& t_Matrix);
};

//单目取反运算符
Matrix operator-(const Matrix t_Matrix);

//矩阵间的运算
Matrix operator+(const Matrix t_Matrix1, const Matrix t_Matrix2);
Matrix operator-(const Matrix t_Matrix1, const Matrix t_Matrix2);
Matrix operator*(const Matrix t_Matrix1, const Matrix t_Matrix2);

//矩阵与复数的运算
Matrix operator+(const Matrix t_Matrix, const Complex t_Complex);
Matrix operator+(const Complex t_Complex, const Matrix t_Matrix);

Matrix operator-(const Matrix t_Matrix, const Complex t_Complex);
Matrix operator-(const Complex t_Complex, const Matrix t_Matrix);

Matrix operator*(const Matrix t_Matrix, const Complex t_Complex);
Matrix operator*(const Complex t_Complex, const Matrix t_Matrix);

Matrix operator/(const Matrix t_Matrix, const Complex t_Complex);
Matrix operator/(const Complex t_Complex, const Matrix t_Matrix);


//对应位置元素计算
Matrix elementProduct(const Matrix t_Matrix1, const Matrix t_Matrix2);
Matrix elementDivide(const Matrix t_Matrix1, const Matrix t_Matrix2);
