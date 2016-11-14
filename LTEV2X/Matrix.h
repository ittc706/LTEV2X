#pragma once
#include<vector>
#include<random>
#include<string>
#include<utility>
#include<iostream>
#include"Complex.h"
#include"Exception.h"

class RowVector {
	/*------------------��------------------*/
private:
	Complex* rowVector;//�ײ���������Ϊ˽�н�ֱֹ�ӷ���
public:
	int col;//�������ȣ��������������������Ϊ����


	/*------------------����------------------*/
public:
	/*---���캯��---*/
	~RowVector();
	RowVector();
	explicit RowVector(int t_Col);
	RowVector(const RowVector& t_RowVector);
	RowVector(RowVector&& t_RowVector) noexcept;
	RowVector(const std::initializer_list<Complex> il);

	/*---��Ա���������---*/
	RowVector& operator=(const RowVector& t_RowVector);
	RowVector& operator=(RowVector&& t_RowVector) noexcept;
	Complex& operator[](int pos);
	const Complex& operator[](int pos) const;

	/*---�������ܺ���---*/
	void resize(int size);
	std::string toString();
	void print(std::ostream&out = std::cout);

private:
	void free();
};
//��������Ŀȡ�������
RowVector operator-(const RowVector& t_RowVector);

//��������������������
RowVector operator+(const RowVector& t_RowVector1, const RowVector& t_RowVector2);

//�������븴��������
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
	/*------------------��̬------------------*/
public:
	/*---��---*/
	static std::default_random_engine s_Engine;
	/*---����---*/
	static Matrix buildDdentityMatrix(int t_Row);
	static Matrix verticalMerge(const Matrix& t_Matrix1, const Matrix& t_Matrix2);
	static std::pair<Matrix, Matrix> verticalSplit(const Matrix& t_Matrix, int leftCol, int rightCol);
	static std::pair<Matrix, Matrix> horizonSplit(const Matrix& t_Matrix, int upRow, int downRow);
	static Matrix eye(const int dim);//���ɵ�λ��
	/*------------------��------------------*/
private:
	RowVector* matrix;//�ײ��ά��������Ϊ˽�н�ֱֹ�ӷ���
public:
	int row;//����
	int col;//����

	/*------------------����------------------*/
	/*---�������ƺ���---*/
public:
	~Matrix();
	Matrix();
	Matrix(int t_Row, int t_Col);
	Matrix(const Matrix& t_Matrix);
	Matrix(Matrix&& t_Matrix) noexcept;
	Matrix(const std::initializer_list<RowVector>& il);
private:
	void free();
	

	/*---�����ܺ���---*/
public:
	void randomFill(double realLeft, double readRight, double imagLeft, double imagRight);
	Matrix conjugate();//����
	Matrix transpose();//��ת��
	Matrix hermitian();//����ת��
	Matrix inverse(bool tryPseudoInverse = false);//�������
	Matrix diag();//��Խ��߹��ɵľ���(������)
	std::pair<Matrix, Matrix> fullRankDecomposition();//�����ȷֽ�
	Matrix pseudoInverse();//����������


	/*---��Ա���������---*/
public:
	Matrix& operator=(const Matrix& t_Matrix);//��ֵ�����
	Matrix& operator=(Matrix&& t_Matrix) noexcept;//��ֵ�����
	RowVector& operator[](int pos);//�±������(�ǳ����汾)
	const RowVector& operator[](int pos) const;//�±������(�����汾)


	/*---�������ܺ���---*/
public:
	std::string toString();
	void print(std::ostream&out = std::cout, int numEnter = 0);


private:
	Matrix inverseWhenDimlowerThan3(bool tryPseudoInverse);
};

//��Ŀȡ�������
Matrix operator-(const Matrix& t_Matrix);

//����������
Matrix operator+(const Matrix& t_Matrix1, const Matrix& t_Matrix2);
Matrix operator-(const Matrix& t_Matrix1, const Matrix& t_Matrix2);
Matrix operator*(const Matrix& t_Matrix1, const Matrix& t_Matrix2);

//�����븴��������
Matrix operator+(const Matrix& t_Matrix, const Complex& t_Complex);
Matrix operator+(const Complex& t_Complex, const Matrix& t_Matrix);

Matrix operator-(const Matrix& t_Matrix, const Complex& t_Complex);
Matrix operator-(const Complex& t_Complex, const Matrix& t_Matrix);

Matrix operator*(const Matrix& t_Matrix, const Complex& t_Complex);
Matrix operator*(const Complex& t_Complex, const Matrix& t_Matrix);

Matrix operator/(const Matrix& t_Matrix, const Complex& t_Complex);
Matrix operator/(const Complex& t_Complex, const Matrix& t_Matrix);


//��Ӧλ��Ԫ�ؼ���
Matrix elementProduct(const Matrix& t_Matrix1, const Matrix& t_Matrix2);
Matrix elementDivide(const Matrix& t_Matrix1, const Matrix& t_Matrix2);
