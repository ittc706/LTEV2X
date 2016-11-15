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
	/*
	* 底层容器，设为私有禁止直接访问
	*/
	Complex* rowVector;
public:
	/*
	* 向量长度，由于是行向量，这里称为列数
	*/
	int col;


	/*------------------方法------------------*/
private:
	/*
	* 资源清理，即释放堆内存
	*/
	void free();
public:
	/*
	* 析构函数，用于释放rowVector
	*/
	~RowVector();

	/*
	* 默认构造函数
	* 向量长度为0，指针为空
	*/
	RowVector();

	/*
	* 接受一个向量维度的参数
	* 初始化后，向量维度等于指定的t_Col，元素初始化微为数值(0,0)
	*/
	explicit RowVector(int t_Col);

	/*
	* 拷贝构造函数
	* 注意有指针成员时，拷贝语意的实现
	*/
	RowVector(const RowVector& t_RowVector);

	/*
	* 移动构造函数
	* 注意移动后源对象指针成员的置空
	* 声明为不抛出异常的
	*/
	RowVector(RowVector&& t_RowVector) noexcept;

	/*
	* 列表构造函数
	*/
	RowVector(const std::initializer_list<Complex> t_InitializeList);


	/*
	* 拷贝赋值运算符
	*/
	RowVector& operator=(const RowVector& t_RowVector);

	/*
	* 移动赋值运算符
	*/
	RowVector& operator=(RowVector&& t_RowVector) noexcept;

	/*
	* 下标运算符
	*/
	Complex& operator[](int t_Pos);

	/*
	* 常量版本的下标运算符
	*/
	const Complex& operator[](int t_Pos) const;

	/*
	* 重置大小
	* 重置后若维度变大，那么补(0,0)
	* 重置后若维度变小，那么删除多余元素
	*/
	void resize(int t_Size);

	/*
	* 生成向量的格式化字符串
	*/
	std::string toString();

	/*
	* 将向量打印到指定输出流
	*/
	void print(std::ostream&t_Out = std::cout);

};

/*
* 向量单目取反运算符
*/
RowVector operator-(const RowVector& t_RowVector);

/*
* 向量加法、减法运算
*/
RowVector operator+(const RowVector& t_RowVector1, const RowVector& t_RowVector2);
RowVector operator-(const RowVector& t_RowVector1, const RowVector& t_RowVector2);

/*
* 向量与复数(复数与向量)的加减乘除运算符
*/
RowVector operator+(const RowVector& t_RowVector, const Complex&t_Complex);
RowVector operator+(const Complex&t_Complex, const RowVector& t_RowVector);

RowVector operator-(const RowVector& t_RowVector, const Complex&t_Complex);
RowVector operator-(const Complex&t_Complex, const RowVector& t_RowVector);

RowVector operator*(const RowVector& t_RowVector, const Complex&t_Complex);
RowVector operator*(const Complex&t_Complex, const RowVector& t_RowVector);

RowVector operator/(const RowVector& t_RowVector, const Complex&t_Complex);
RowVector operator/(const Complex&t_Complex, const RowVector& t_RowVector);

/*
* 向量与向量的对应元素乘法与除法
*/
RowVector elementProduct(const RowVector& t_RowVector1, const RowVector& t_RowVector2);
RowVector elementDivide(const RowVector& t_RowVector1, const RowVector& t_RowVector2);


class Matrix {
	/*------------------静态------------------*/
public:
	/*
	* 随机数引擎，该类共享
	*/
	static std::default_random_engine s_Engine;

	/*
	* 矩阵的上下合并，因此列维度必须相同
	*/
	static Matrix verticalMerge(const Matrix& t_Matrix1, const Matrix& t_Matrix2);

	/*
	* 矩阵的垂直分裂
	*/
	static std::pair<Matrix, Matrix> verticalSplit(const Matrix& t_Matrix, int t_LeftCol, int t_RightCol);

	/*
	* 矩阵的水平分裂
	*/
	static std::pair<Matrix, Matrix> horizonSplit(const Matrix& t_Matrix, int t_UpRow, int t_DownRow);

	/*
	* 生成指定维度的单位矩阵
	*/
	static Matrix eye(const int t_Dim);
	/*------------------域------------------*/
private:
	/*
	* 底层向量容器，设为私有禁止直接访问
	*/
	RowVector* matrix;
public:
	/*
	* 行数
	*/
	int row;

	/*
	* 列数
	*/
	int col;

	/*------------------方法------------------*/
private:
	/*
	* 清理堆内存
	*/
	void free();
public:
	/*
	* 析构函数
	*/
	~Matrix();

	/*
	* 默认构造函数
	* 行列均为0，指针为空
	*/
	Matrix();

	/*
	* 指定维度的构造函数
	*/
	Matrix(int t_Row, int t_Col);

	/*
	* 拷贝构造函数，注意有指针时拷贝语意的实现
	*/
	Matrix(const Matrix& t_Matrix);

	/*
	* 移动构造函数，注意有指针时移动语意的实现
	*/
	Matrix(Matrix&& t_Matrix) noexcept;

	/*
	* 列表构造函数
	* 列表中每行的元素数目可以不同
	* 以最长的一行的维度作为列数
	* 不足的补上(0,0)
	*/
	Matrix(const std::initializer_list<RowVector>& t_InitializeList);

	/*---矩阵功能函数---*/
public:
	/*
	* 随机填充均匀分布的数值
	* 实部的范围[t_RealLeft,t_RealRight]
	* 虚部的范围[t_ImagLeft,t_ImagRight]
	*/
	void randomFill(double t_RealLeft, double t_RealRight, double t_ImagLeft, double t_ImagRight);

	/*
	* 求共轭
	*/
	Matrix conjugate();

	/*
	* 求转置
	*/
	Matrix transpose();

	/*
	* 共轭转置
	*/
	Matrix hermitian();

	/*
	* 求逆矩阵
	* 当布尔值为true时，当矩阵无法求逆，会尝试求伪逆(广义逆矩阵)
	*/
	Matrix inverse(bool t_TryPseudoInverse = false);

	/*
	* 求对角线构成的矩阵(行向量)
	*/
	Matrix diag();

	/*
	* 求满秩分解
	*/
	std::pair<Matrix, Matrix> fullRankDecomposition();

	/*
	* 求广义逆矩阵
	*/
	Matrix pseudoInverse();


	/*---成员运算符重载---*/
public:
	/*
	* 拷贝赋值运算符
	* 注意，有指针成员时，也需要检查自赋值的正确性
	* 注意原指针析构的时机
	*/
	Matrix& operator=(const Matrix& t_Matrix);

	/*
	* 移动赋值运算符
	* 需要检查自赋值的正确性
	* 注意原指针析构的时机
	* 声明为不会抛出异常
	*/
	Matrix& operator=(Matrix&& t_Matrix) noexcept;

	/*
	* 下标运算符(非常量版本)
	*/
	RowVector& operator[](int t_Pos);

	/*
	* 下标运算符(常量版本)
	*/
	const RowVector& operator[](int t_Pos) const;


	/*---其他功能函数---*/
public:
	/*
	* 生成格式化字符串
	*/
	std::string toString();

	/*
	* 打印到指定输出流
	*/
	void print(std::ostream&t_Out = std::cout, int t_NumEnter = 0);


private:
	/*
	* 用于计算当维度小于3时的矩阵求逆运算
	* 同理，当布尔值为true时，无法求逆便会求伪逆(广义逆矩阵)
	*/
	Matrix inverseWhenDimlowerThan3(bool t_TryPseudoInverse);
};

/*
* 单目取反运算符
*/
Matrix operator-(const Matrix& t_Matrix);


/*
* 矩阵间的运算，即加、减、乘
*/
Matrix operator+(const Matrix& t_Matrix1, const Matrix& t_Matrix2);
Matrix operator-(const Matrix& t_Matrix1, const Matrix& t_Matrix2);
Matrix operator*(const Matrix& t_Matrix1, const Matrix& t_Matrix2);

/*
* 矩阵与复数的运算，即加、减、乘、除
*/
Matrix operator+(const Matrix& t_Matrix, const Complex& t_Complex);
Matrix operator+(const Complex& t_Complex, const Matrix& t_Matrix);

Matrix operator-(const Matrix& t_Matrix, const Complex& t_Complex);
Matrix operator-(const Complex& t_Complex, const Matrix& t_Matrix);

Matrix operator*(const Matrix& t_Matrix, const Complex& t_Complex);
Matrix operator*(const Complex& t_Complex, const Matrix& t_Matrix);

Matrix operator/(const Matrix& t_Matrix, const Complex& t_Complex);
Matrix operator/(const Complex& t_Complex, const Matrix& t_Matrix);


/*
* 对应位置元素计算
*/
Matrix elementProduct(const Matrix& t_Matrix1, const Matrix& t_Matrix2);
Matrix elementDivide(const Matrix& t_Matrix1, const Matrix& t_Matrix2);
