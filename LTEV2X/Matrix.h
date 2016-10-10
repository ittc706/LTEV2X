#pragma once
#include<vector>
#include<random>
#include<string>
#include<utility>
#include<iostream>
#include"Complex.h"
#include"Exception.h"

/*
* ��������Ŀ����Ϊ����Խ����ʵ�ʱ���׳��쳣�����ǲ���һ������ʱ���󣬸����ڴ���Ķ�λ
* vector�±��Խ�����
*/
class RowVector {
	/*���ݳ�Ա*/
	std::vector<Complex> rowVector;//�ײ���������Ϊ˽�н�ֱֹ�ӷ���
public:
	/*���ݳ�Ա*/
	int col;//�������ȣ��������������������Ϊ����

	/*���캯��*/
	RowVector();
	explicit RowVector(int t_Col);
	RowVector(const RowVector& t_RowVector);
	RowVector(const std::initializer_list<Complex> il);

	/*��Ա���������*/
	RowVector& operator=(const RowVector& t_RowVector);
	Complex& operator[](int pos);
	const Complex& operator[](int pos) const;

	/*�������ܺ���*/
	void resize(int size);
	std::string toString();
	void print(std::ostream&out = std::cout);
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
	/*���ݳ�Ա*/
	std::vector<RowVector> matrix;//�ײ��ά��������Ϊ˽�н�ֱֹ�ӷ���
public:
	/*��̬���ݳ�Ա*/
	static std::default_random_engine s_Engine;

	/*���ݳ�Ա*/
	int row;//����
	int col;//����


	/*���캯��*/
	Matrix();
	Matrix(int t_Row, int t_Col);
	Matrix(const Matrix& t_Matrix);
	Matrix(const std::initializer_list<RowVector> il);
	

	/*�����ܺ���*/
	void randomFill(double realLeft, double readRight, double imagLeft, double imagRight);
	Matrix conjugate();//����
	Matrix transpose();//��ת��
	Matrix hermitian();//����ת��
	Matrix inverse();//�������
	Matrix diag();//��Խ��߹��ɵľ���(������)


	/*��Ա���������*/
	Matrix& operator=(const Matrix& t_Matrix);//��ֵ�����
	RowVector& operator[](int pos);//�±������(�ǳ����汾)
	const RowVector& operator[](int pos) const;//�±������(�����汾)


	/*�������ܺ���*/
	std::string toString();
	void print(std::ostream&out = std::cout);


	/*���ڹ������ľ�̬����*/
	static Matrix buildDdentityMatrix(int t_Row);

	/*ʵ�ֺ���*/
	static Matrix merge(const Matrix& t_Matrix1, const Matrix& t_Matrix2);
	static std::pair<Matrix, Matrix> split(const Matrix& t_Matrix);
};

//��Ŀȡ�������
Matrix operator-(const Matrix t_Matrix);

//����������
Matrix operator+(const Matrix t_Matrix1, const Matrix t_Matrix2);
Matrix operator-(const Matrix t_Matrix1, const Matrix t_Matrix2);
Matrix operator*(const Matrix t_Matrix1, const Matrix t_Matrix2);

//�����븴��������
Matrix operator+(const Matrix t_Matrix, const Complex t_Complex);
Matrix operator+(const Complex t_Complex, const Matrix t_Matrix);

Matrix operator-(const Matrix t_Matrix, const Complex t_Complex);
Matrix operator-(const Complex t_Complex, const Matrix t_Matrix);

Matrix operator*(const Matrix t_Matrix, const Complex t_Complex);
Matrix operator*(const Complex t_Complex, const Matrix t_Matrix);

Matrix operator/(const Matrix t_Matrix, const Complex t_Complex);
Matrix operator/(const Complex t_Complex, const Matrix t_Matrix);


//��Ӧλ��Ԫ�ؼ���
Matrix elementProduct(const Matrix t_Matrix1, const Matrix t_Matrix2);
Matrix elementDivide(const Matrix t_Matrix1, const Matrix t_Matrix2);
