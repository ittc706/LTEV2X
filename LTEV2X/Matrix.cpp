/*
* =====================================================================================
*
*       Filename:  Matrix.cpp
*
*    Description:  Matrix实现
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


#include<iostream>
#include<iomanip>
#include<sstream>
#include<cmath>
#include"Matrix.h"


using namespace std;


RowVector::RowVector() :
	col(0) {}


RowVector::RowVector(int t_Col) :
	col(t_Col) {
	if (col < 0) throw Exp("向量的维度必须是非负的");
	rowVector = vector<Complex>(col, Complex(0, 0));
}


RowVector::RowVector(const RowVector& t_RowVector) :
	col(t_RowVector.col), rowVector(t_RowVector.rowVector) {}



RowVector::RowVector(const std::initializer_list<Complex> il) {
	col = static_cast<int>(il.size());
	for (const Complex&c : il)
		rowVector.push_back(c);
}


RowVector& RowVector::operator=(const RowVector& t_RowVector) {
	col = t_RowVector.col;
	rowVector = t_RowVector.rowVector;
	return *this;
}


Complex& RowVector::operator[](int pos) {
	return rowVector.at(pos);
}


const Complex& RowVector::operator[](int pos) const {
	return rowVector.at(pos);
}


void RowVector::resize(int size) {
	if (size < 0) throw Exp("向量的维度必须是非负的");
	rowVector.resize(size, Complex(0, 0));
	col = size;
}


std::string RowVector::toString() {
	ostringstream ss;
	for (int c = 0; c < col; c++) {
		ss << this->operator[](c).toString() << "  ";
	}
	return ss.str();
}


void RowVector::print(std::ostream&out) {
	out << toString();
}

//行向量单目取反运算符
RowVector operator-(const RowVector& t_RowVector) {
	RowVector res(t_RowVector.col);
	for (int c = 0; c < t_RowVector.col; c++) {
		res[c] = -t_RowVector[c];
	}
	return res;
}


//行向量与行向量的运算
RowVector operator+(const RowVector& t_RowVector1, const RowVector& t_RowVector2) {
	if (t_RowVector1.col != t_RowVector2.col) throw Exp("向量维度不同，无法相加");
	RowVector res(t_RowVector1.col);
	for (int c = 0; c < t_RowVector1.col; c++) {
		res[c] = t_RowVector1[c] + t_RowVector2[c];
	}
	return res;
}


//行向量与复数的运算
RowVector operator+(const RowVector& t_RowVector, const Complex&t_Complex) {
	RowVector res(t_RowVector.col);
	for (int c = 0; c < t_RowVector.col; c++) {
		res[c] = t_RowVector[c] + t_Complex;
	}
	return res;
}
RowVector operator+(const Complex&t_Complex, const RowVector& t_RowVector) {
	return t_RowVector + t_Complex;
}


RowVector operator-(const RowVector& t_RowVector, const Complex&t_Complex) {
	RowVector res(t_RowVector.col);
	for (int c = 0; c < t_RowVector.col; c++) {
		res[c] = t_RowVector[c] - t_Complex;
	}
	return res;
}
RowVector operator-(const Complex&t_Complex, const RowVector& t_RowVector) {
	return -t_RowVector + t_Complex;
}


RowVector operator*(const RowVector& t_RowVector, const Complex&t_Complex) {
	RowVector res(t_RowVector.col);
	for (int c = 0; c < t_RowVector.col; c++) {
		res[c] = t_RowVector[c] * t_Complex;
	}
	return res;
}
RowVector operator*(const Complex&t_Complex, const RowVector& t_RowVector) {
	return t_RowVector*t_Complex;
}


RowVector operator/(const RowVector& t_RowVector, const Complex&t_Complex) {
	if (Complex::abs(t_Complex) == 0) throw Exp("除数模值不能为0");
	RowVector res(t_RowVector.col);
	for (int c = 0; c < t_RowVector.col; c++) {
		res[c] = t_RowVector[c] / t_Complex;
	}
	return res;
}
RowVector operator/(const Complex&t_Complex, const RowVector& t_RowVector) {
	RowVector res(t_RowVector.col);
	for (int c = 0; c < t_RowVector.col; c++) {
		if (Complex::abs(t_RowVector[c]) == 0) throw Exp("向量中含有模值为0的元素，不能作为分母");
		res[c] = t_Complex / t_RowVector[c];
	}
	return res;
}


RowVector elementProduct(const RowVector& t_RowVector1, const RowVector& t_RowVector2) {
	if (t_RowVector1.col != t_RowVector2.col) throw Exp("向量维度不匹配");
	RowVector res(t_RowVector1.col);
	for (int c = 0; c < t_RowVector1.col; c++) {
		res[c] = t_RowVector1[c] * t_RowVector2[c];
	}
	return res;
}
RowVector elementDivide(const RowVector& t_RowVector1, const RowVector& t_RowVector2) {
	if (t_RowVector1.col != t_RowVector2.col) throw Exp("向量维度不匹配");
	RowVector res(t_RowVector1.col);
	for (int c = 0; c < t_RowVector1.col; c++) {
		if (Complex::abs(t_RowVector2[c]) == 0) throw Exp("元素模值为0，不可作为除数");
		res[c] = t_RowVector1[c] / t_RowVector2[c];
	}
	return res;
}


default_random_engine Matrix::s_Engine((unsigned)time(NULL));


Matrix::Matrix() :
	row(0), col(0) {}



Matrix::Matrix(int t_Row, int t_Col) :
	row(t_Row), col(t_Col) {
	if (t_Row < 0 || t_Col < 0) throw Exp("矩阵的维度必须是非负的");
	matrix = vector<RowVector>(row, RowVector(col));
}


Matrix::Matrix(const Matrix& t_Matrix) :
	row(t_Matrix.row), col(t_Matrix.col), matrix(t_Matrix.matrix) {}


Matrix::Matrix(const std::initializer_list<RowVector> il) {
	if (il.size() == 0) {
		row = 0;
		col = 0;
		matrix = vector<RowVector>(row, RowVector(col));
	}
	else {
		col = 0;
		row = static_cast<int>(il.size());
		for (const RowVector rv : il) {
			if (col < rv.col) col = rv.col;
		}
		for (const RowVector rv : il) {
			RowVector rvTmp(rv);
			rvTmp.resize(col);
			matrix.push_back(rvTmp);
		}
	}
}


void Matrix::randomFill(double realLeft, double readRight, double imagLeft, double imagRight) {
	uniform_real_distribution<double> urdReal(realLeft, readRight);
	uniform_real_distribution<double> urdImag(imagLeft, imagRight);
	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
			matrix[r][c] = Complex(urdReal(Matrix::s_Engine), urdImag(Matrix::s_Engine));
		}
	}
}


Matrix Matrix::conjugate() {
	Matrix res(row, col);
	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
			res[r][c] = matrix[r][c].conjugate();
		}
	}
	return res;
}


Matrix Matrix::transpose() {
	Matrix res(col, row);
	for (int r = 0; r < col; r++) {
		for (int c = 0; c < row; c++) {
			res[r][c] = matrix[c][r];
		}
	}
	return res;
}


Matrix Matrix::hermitian() {
	Matrix res = conjugate();
	res = res.transpose();
	return res;
}


Matrix Matrix::inverse() {
	if (row <= 0 || col <= 0 || row != col) throw Exp("该矩阵无法求逆");
	Matrix mergeMatrix = Matrix::merge(*this, Matrix::buildDdentityMatrix(row));

	//先变换成下三角矩阵
	Complex zero(0, 0);
	int tmpRow = 0;
	RowVector tmpRV;
	for (int r = 0; r < row; r++) {
		if (mergeMatrix[r][r] == zero) {//若当前行的对角线部分为0，则与不为0的那行互换
			tmpRow = r + 1;
			while (tmpRow < row&&mergeMatrix[tmpRow][r] == zero)
				tmpRow++;
			if (tmpRow == row) throw Exp("该矩阵不满秩");

			//互换r与tmp两行
			tmpRV = mergeMatrix[r];
			mergeMatrix[r] = mergeMatrix[tmpRow];
			mergeMatrix[tmpRow] = tmpRV;
		}
		mergeMatrix[r] = mergeMatrix[r] / mergeMatrix[r][r];//将对角线部分置1

		//将该列的下半部分置0
		for (tmpRow = r + 1; tmpRow < row; tmpRow++) {
			if (mergeMatrix[tmpRow][r] == zero)continue;
			Complex factor = -mergeMatrix[tmpRow][r];
			mergeMatrix[tmpRow] = mergeMatrix[r] * factor + mergeMatrix[tmpRow];
		}
	}


	//将右边继续变换为上三角
	for (int r = row - 1; r >= 0; r--) {
		for (tmpRow = r - 1; tmpRow >= 0; tmpRow--) {
			if (mergeMatrix[tmpRow][r] == zero)continue;
			Complex factor = -mergeMatrix[tmpRow][r];
			mergeMatrix[tmpRow] = mergeMatrix[r] * factor + mergeMatrix[tmpRow];
		}
	}
	return Matrix::split(mergeMatrix).second;
}


Matrix Matrix::diag() {
	if (row != col) throw Exp("该函数只支持方阵");
	Matrix res(1,row);
	for (int r = 0; r < row; r++) {
		res[0][r] = this->operator[](r)[r];
	}
	return res;
}


Matrix& Matrix::operator=(const Matrix& t_Matrix) {
	row = t_Matrix.row;
	col = t_Matrix.col;
	matrix = t_Matrix.matrix;
	return *this;
}


RowVector& Matrix::operator[](int pos) {
	return matrix.at(pos);
}


const RowVector& Matrix::operator[](int pos) const {
	return  matrix.at(pos);
}


std::string Matrix::toString() {
	ostringstream ss;
	for (int r = 0; r < row; r++) {
		ss << this->operator[](r).toString() << endl;
	}
	return ss.str();
}


void Matrix::print(std::ostream&out) {
	out << toString();
}


Matrix Matrix::buildDdentityMatrix(int t_Row) {
	Matrix res(t_Row, t_Row);
	for (int r = 0; r < t_Row; r++)
		res[r][r] = Complex(1, 0);
	return res;
}


Matrix Matrix::merge(const Matrix& t_Matrix1, const Matrix& t_Matrix2) {
	if (t_Matrix1.row != t_Matrix2.row || t_Matrix1.col != t_Matrix2.col) throw Exp("这两个矩阵无法合并");
	Matrix res(t_Matrix1.row, t_Matrix1.col * 2);
	for (int r = 0; r < t_Matrix1.row; r++) {
		for (int c = 0; c < t_Matrix1.col; c++) {
			res[r][c] = t_Matrix1[r][c];
			res[r][c + t_Matrix1.col] = t_Matrix2[r][c];
		}
	}
	return res;
}


std::pair<Matrix, Matrix> Matrix::split(const Matrix& t_Matrix) {
	if (t_Matrix.col % 2 != 0) throw Exp("该矩阵无法分裂");
	Matrix left(t_Matrix.row, t_Matrix.col / 2), right(t_Matrix.row, t_Matrix.col / 2);
	for (int r = 0; r < left.row; r++) {
		for (int c = 0; c < left.col; c++) {
			left[r][c] = t_Matrix[r][c];
			right[r][c] = t_Matrix[r][c + left.col];
		}
	}
	return pair<Matrix, Matrix>(left, right);
}


//单目取反运算符
Matrix operator-(const Matrix t_Matrix) {
	Matrix res(t_Matrix.row, t_Matrix.col);
	for (int r = 0; r < t_Matrix.row; r++) {
		res[r] = -t_Matrix[r];
	}
	return res;
}


//矩阵间的运算
Matrix operator+(const Matrix t_Matrix1, const Matrix t_Matrix2) {
	if (t_Matrix1.row != t_Matrix2.row || t_Matrix1.col != t_Matrix2.col) throw Exp("矩阵维度不同，无法相加");
	Matrix res(t_Matrix1.row, t_Matrix1.col);
	for (int r = 0; r < t_Matrix1.row; r++) {
		for (int c = 0; c < t_Matrix1.col; c++) {
			res[r][c] = t_Matrix1[r][c] + t_Matrix2[r][c];
		}
	}
	return res;
}


Matrix operator-(const Matrix t_Matrix1, const Matrix t_Matrix2) {
	if (t_Matrix1.row != t_Matrix2.row || t_Matrix1.col != t_Matrix2.col) throw Exp("矩阵维度不同，无法相减");
	Matrix res(t_Matrix1.row, t_Matrix1.col);
	for (int r = 0; r < t_Matrix1.row; r++) {
		for (int c = 0; c < t_Matrix1.col; c++) {
			res[r][c] = t_Matrix1[r][c] - t_Matrix2[r][c];
		}
	}
	return res;
}


Matrix operator*(const Matrix t_Matrix1, const Matrix t_Matrix2) {
	if (t_Matrix1.col != t_Matrix2.row) throw Exp("矩阵维度不匹配，无法相乘");

	Matrix res(t_Matrix1.row, t_Matrix2.col);

	for (int r = 0; r < t_Matrix1.row; r++) {
		for (int c = 0; c < t_Matrix2.col; c++) {
			//计算每一项
			Complex tmp(0, 0);
			for (int idx = 0; idx < t_Matrix1.col; idx++) {
				tmp += t_Matrix1[r][idx] * t_Matrix2[idx][c];
			}
			res[r][c] = tmp;
		}
	}
	return res;
}


//矩阵与复数的运算
Matrix operator+(const Matrix t_Matrix, const Complex t_Complex) {
	Matrix res(t_Matrix.row, t_Matrix.col);
	for (int r = 0; r < t_Matrix.row; r++) {
		res[r] = t_Matrix[r] + t_Complex;
	}
	return res;
}
Matrix operator+(const Complex t_Complex, const Matrix t_Matrix) {
	return t_Matrix + t_Complex;
}


Matrix operator-(const Matrix t_Matrix, const Complex t_Complex) {
	Matrix res(t_Matrix.row, t_Matrix.col);
	for (int r = 0; r < t_Matrix.row; r++) {
		res[r] = t_Matrix[r] - t_Complex;
	}
	return res;
}
Matrix operator-(const Complex t_Complex, const Matrix t_Matrix) {
	return -t_Matrix + t_Complex;
}


Matrix operator*(const Matrix t_Matrix, const Complex t_Complex) {
	Matrix res(t_Matrix.row, t_Matrix.col);
	for (int r = 0; r < t_Matrix.row; r++) {
		res[r] = t_Matrix[r] * t_Complex;
	}
	return res;
}
Matrix operator*(const Complex t_Complex, const Matrix t_Matrix) {
	return t_Matrix*t_Complex;
}


Matrix operator/(const Matrix t_Matrix, const Complex t_Complex) {
	if (Complex::abs(t_Complex) == 0) throw Exp("除数模值为0，非法！");
	Matrix res(t_Matrix.row, t_Matrix.col);
	for (int r = 0; r < t_Matrix.row; r++) {
		res[r] = t_Matrix[r] / t_Complex;
	}
	return res;
}
Matrix operator/(const Complex t_Complex, const Matrix t_Matrix) {
	Matrix res(t_Matrix.row, t_Matrix.col);
	for (int r = 0; r < t_Matrix.row; r++) {
		res[r] = t_Complex / t_Matrix[r];
	}
	return res;
}


Matrix elementProduct(const Matrix t_Matrix1, const Matrix t_Matrix2) {
	if (t_Matrix1.row != t_Matrix2.row ||
		t_Matrix1.col != t_Matrix2.col) throw Exp("矩阵维度不匹配");
	Matrix res(t_Matrix1.row, t_Matrix1.col);
	for (int r = 0; r < t_Matrix1.row; r++) {
		res[r] = elementProduct(t_Matrix1[r], t_Matrix2[r]);
	}
	return res;
}
Matrix elementDivide(const Matrix t_Matrix1, const Matrix t_Matrix2) {
	if (t_Matrix1.row != t_Matrix2.row ||
		t_Matrix1.col != t_Matrix2.col) throw Exp("矩阵维度不匹配");
	Matrix res(t_Matrix1.row, t_Matrix1.col);
	for (int r = 0; r < t_Matrix1.row; r++) {
		res[r] = elementDivide(t_Matrix1[r], t_Matrix2[r]);
	}
	return res;
}