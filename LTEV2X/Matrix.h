#pragma once
#include<vector>
#include<random>
#include<string>
#include<utility>
#include<iostream>
#include"Complex.h"
#include"Exception.h"

class RowVector {
	/*------------------域------------------*/
private:
	Complex* rowVector;//底层容器，设为私有禁止直接访问
public:
	int col;//向量长度，由于是行向量，这里称为列数


	/*------------------方法------------------*/
public:
	/*---构造函数---*/
	~RowVector();
	RowVector();
	explicit RowVector(int t_Col);
	RowVector(const RowVector& t_RowVector);
	RowVector(RowVector&& t_RowVector) noexcept;
	RowVector(const std::initializer_list<Complex> il);

	/*---成员运算符重载---*/
	RowVector& operator=(const RowVector& t_RowVector);
	RowVector& operator=(RowVector&& t_RowVector) noexcept;
	Complex& operator[](int pos);
	const Complex& operator[](int pos) const;

	/*---其他功能函数---*/
	void resize(int size);
	std::string toString();
	void print(std::ostream&out = std::cout);

private:
	void free();
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
	/*------------------静态------------------*/
public:
	/*---域---*/
	static std::default_random_engine s_Engine;
	/*---方法---*/
	static Matrix buildDdentityMatrix(int t_Row);
	static Matrix verticalMerge(const Matrix& t_Matrix1, const Matrix& t_Matrix2);
	static std::pair<Matrix, Matrix> verticalSplit(const Matrix& t_Matrix, int leftCol, int rightCol);
	static std::pair<Matrix, Matrix> horizonSplit(const Matrix& t_Matrix, int upRow, int downRow);
	static Matrix eye(const int dim);//生成单位阵
	/*------------------域------------------*/
private:
	RowVector* matrix;//底层多维容器，设为私有禁止直接访问
public:
	int row;//行数
	int col;//列数

	/*------------------方法------------------*/
	/*---拷贝控制函数---*/
public:
	~Matrix();
	Matrix();
	Matrix(int t_Row, int t_Col);
	Matrix(const Matrix& t_Matrix);
	Matrix(Matrix&& t_Matrix) noexcept;
	Matrix(const std::initializer_list<RowVector>& il);
private:
	void free();
	

	/*---矩阵功能函数---*/
public:
	void randomFill(double realLeft, double readRight, double imagLeft, double imagRight);
	Matrix conjugate();//求共轭
	Matrix transpose();//求转置
	Matrix hermitian();//共轭转置
	Matrix inverse(bool tryPseudoInverse = false);//求逆矩阵
	Matrix diag();//求对角线构成的矩阵(行向量)
	std::pair<Matrix, Matrix> fullRankDecomposition();//求满秩分解
	Matrix pseudoInverse();//求广义逆矩阵


	/*---成员运算符重载---*/
public:
	Matrix& operator=(const Matrix& t_Matrix);//赋值运算符
	Matrix& operator=(Matrix&& t_Matrix) noexcept;//赋值运算符
	RowVector& operator[](int pos);//下标运算符(非常量版本)
	const RowVector& operator[](int pos) const;//下标运算符(常量版本)


	/*---其他功能函数---*/
public:
	std::string toString();
	void print(std::ostream&out = std::cout, int numEnter = 0);


private:
	Matrix inverseWhenDimlowerThan3(bool tryPseudoInverse);
};

//单目取反运算符
Matrix operator-(const Matrix& t_Matrix);

//矩阵间的运算
Matrix operator+(const Matrix& t_Matrix1, const Matrix& t_Matrix2);
Matrix operator-(const Matrix& t_Matrix1, const Matrix& t_Matrix2);
Matrix operator*(const Matrix& t_Matrix1, const Matrix& t_Matrix2);

//矩阵与复数的运算
Matrix operator+(const Matrix& t_Matrix, const Complex& t_Complex);
Matrix operator+(const Complex& t_Complex, const Matrix& t_Matrix);

Matrix operator-(const Matrix& t_Matrix, const Complex& t_Complex);
Matrix operator-(const Complex& t_Complex, const Matrix& t_Matrix);

Matrix operator*(const Matrix& t_Matrix, const Complex& t_Complex);
Matrix operator*(const Complex& t_Complex, const Matrix& t_Matrix);

Matrix operator/(const Matrix& t_Matrix, const Complex& t_Complex);
Matrix operator/(const Complex& t_Complex, const Matrix& t_Matrix);


//对应位置元素计算
Matrix elementProduct(const Matrix& t_Matrix1, const Matrix& t_Matrix2);
Matrix elementDivide(const Matrix& t_Matrix1, const Matrix& t_Matrix2);
