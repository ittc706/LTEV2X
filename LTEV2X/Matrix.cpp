/*
* =====================================================================================
*
*       Filename:  Matrix.cpp
*
*    Description:  Matrixʵ��
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
	if (col < 0) throw Exp("������ά�ȱ����ǷǸ���");
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
	if (size < 0) throw Exp("������ά�ȱ����ǷǸ���");
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

//��������Ŀȡ�������
RowVector operator-(const RowVector& t_RowVector) {
	RowVector res(t_RowVector.col);
	for (int c = 0; c < t_RowVector.col; c++) {
		res[c] = -t_RowVector[c];
	}
	return res;
}


//��������������������
RowVector operator+(const RowVector& t_RowVector1, const RowVector& t_RowVector2) {
	if (t_RowVector1.col != t_RowVector2.col) throw Exp("����ά�Ȳ�ͬ���޷����");
	RowVector res(t_RowVector1.col);
	for (int c = 0; c < t_RowVector1.col; c++) {
		res[c] = t_RowVector1[c] + t_RowVector2[c];
	}
	return res;
}


//�������븴��������
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
	if (Complex::abs(t_Complex) == 0) throw Exp("����ģֵ����Ϊ0");
	RowVector res(t_RowVector.col);
	for (int c = 0; c < t_RowVector.col; c++) {
		res[c] = t_RowVector[c] / t_Complex;
	}
	return res;
}
RowVector operator/(const Complex&t_Complex, const RowVector& t_RowVector) {
	RowVector res(t_RowVector.col);
	for (int c = 0; c < t_RowVector.col; c++) {
		if (Complex::abs(t_RowVector[c]) == 0) throw Exp("�����к���ģֵΪ0��Ԫ�أ�������Ϊ��ĸ");
		res[c] = t_Complex / t_RowVector[c];
	}
	return res;
}


RowVector elementProduct(const RowVector& t_RowVector1, const RowVector& t_RowVector2) {
	if (t_RowVector1.col != t_RowVector2.col) throw Exp("����ά�Ȳ�ƥ��");
	RowVector res(t_RowVector1.col);
	for (int c = 0; c < t_RowVector1.col; c++) {
		res[c] = t_RowVector1[c] * t_RowVector2[c];
	}
	return res;
}
RowVector elementDivide(const RowVector& t_RowVector1, const RowVector& t_RowVector2) {
	if (t_RowVector1.col != t_RowVector2.col) throw Exp("����ά�Ȳ�ƥ��");
	RowVector res(t_RowVector1.col);
	for (int c = 0; c < t_RowVector1.col; c++) {
		if (Complex::abs(t_RowVector2[c]) == 0) throw Exp("Ԫ��ģֵΪ0��������Ϊ����");
		res[c] = t_RowVector1[c] / t_RowVector2[c];
	}
	return res;
}


default_random_engine Matrix::s_Engine((unsigned)time(NULL));


Matrix::Matrix() :
	row(0), col(0) {}



Matrix::Matrix(int t_Row, int t_Col) :
	row(t_Row), col(t_Col) {
	if (t_Row < 0 || t_Col < 0) throw Exp("�����ά�ȱ����ǷǸ���");
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


Matrix Matrix::inverse(bool tryPseudoInverse) {
	if (row <= 0 || col <= 0 || row != col) throw Exp("�þ����޷�����");
	Matrix mergeMatrix = Matrix::verticalMerge(*this, Matrix::buildDdentityMatrix(row));

	//�ȱ任�������Ǿ���
	Complex zero(0, 0);
	int tmpRow = 0;
	RowVector tmpRV;
	for (int r = 0; r < row; r++) {
		if (mergeMatrix[r][r] == zero) {//����ǰ�еĶԽ��߲���Ϊ0�����벻Ϊ0�����л���
			tmpRow = r + 1;
			while (tmpRow < row&&mergeMatrix[tmpRow][r] == zero)
				tmpRow++;
			if (tmpRow == row) {
				if (tryPseudoInverse) {
					return this->pseudoInverse();
				}
				else {
					throw Exp("�þ������ȣ��޷��������");
				}
			}

			//����r��tmp����
			tmpRV = mergeMatrix[r];
			mergeMatrix[r] = mergeMatrix[tmpRow];
			mergeMatrix[tmpRow] = tmpRV;
		}
		mergeMatrix[r] = mergeMatrix[r] / mergeMatrix[r][r];//���Խ��߲�����1

		//�����е��°벿����0
		for (tmpRow = r + 1; tmpRow < row; tmpRow++) {
			if (mergeMatrix[tmpRow][r] == zero)continue;
			Complex factor = -mergeMatrix[tmpRow][r];
			mergeMatrix[tmpRow] = mergeMatrix[r] * factor + mergeMatrix[tmpRow];
		}
	}


	//���ұ߼����任Ϊ������
	for (int r = row - 1; r >= 0; r--) {
		for (tmpRow = r - 1; tmpRow >= 0; tmpRow--) {
			if (mergeMatrix[tmpRow][r] == zero)continue;
			Complex factor = -mergeMatrix[tmpRow][r];
			mergeMatrix[tmpRow] = mergeMatrix[r] * factor + mergeMatrix[tmpRow];
		}
	}
	return Matrix::verticalSplit(mergeMatrix, col, col).second;
}


Matrix Matrix::diag() {
	if (row != col) throw Exp("�ú���ֻ֧�ַ���");
	Matrix res(1,row);
	for (int r = 0; r < row; r++) {
		res[0][r] = this->operator[](r)[r];
	}
	return res;
}


std::pair<Matrix, Matrix>  Matrix::fullRankDecomposition() {
	//������ά��Ϊm*n����Ϊr��r����Ϊ1
	Matrix mergeMatrix = Matrix::verticalMerge(*this, Matrix::buildDdentityMatrix(row));

	//���Ƚ�����ת��Ϊ��׼�ͣ���������r*n�������Ⱦ���������(m-r)*n�������
	Complex zero(0, 0);
	int tmpRow = 0;
	RowVector tmpRV;
	int iterCol = 0;
	int rank = 0;
	for (int r = 0; r < row; r++) {
		if (iterCol == col) break;
		if (mergeMatrix[r][iterCol] == zero) {//����ǰ�еĶԽ��߲���Ϊ0�����벻Ϊ0�����л���
			tmpRow = r + 1;
			while (tmpRow < row&&mergeMatrix[tmpRow][iterCol] == zero)
				tmpRow++;
			if (tmpRow == row) { //������û�з���Ԫ�أ���������
				iterCol++;
				r--;
				continue;//�����ڵ�ǰ�е���һ��Ѱ�ҷ���Ԫ��
			}
			else {
				//����r��tmp����
				tmpRV = mergeMatrix[r];
				mergeMatrix[r] = mergeMatrix[tmpRow];
				mergeMatrix[tmpRow] = tmpRV;
			}	
		}
		mergeMatrix[r] = mergeMatrix[r] / mergeMatrix[r][iterCol];//���Խ��߲�����1

		//�����е��°벿����0
		for (tmpRow = r + 1; tmpRow < row; tmpRow++) {
			if (mergeMatrix[tmpRow][iterCol] == zero)continue;
			Complex factor = -mergeMatrix[tmpRow][iterCol];
			mergeMatrix[tmpRow] = mergeMatrix[iterCol] * factor + mergeMatrix[tmpRow];
		}
		iterCol++;
		rank++;
	}
    
	if (rank == 0) throw Exp("�þ�����Ϊ0�����������ȷֽ������");

	pair<Matrix, Matrix> splitRes = verticalSplit(mergeMatrix, mergeMatrix.col - row, row);
	Matrix B = splitRes.first;
	Matrix P = splitRes.second;

	Matrix F = verticalSplit(P.inverse(),rank, P.col - rank).first;
	Matrix G = horizonSplit(B, rank, B.row - rank).first;

	return pair<Matrix, Matrix>(F, G);
}


Matrix Matrix::pseudoInverse() {
	pair<Matrix, Matrix> fullRankDecompositionRes = fullRankDecomposition();
	Matrix F = fullRankDecompositionRes.first;
	Matrix G = fullRankDecompositionRes.second;
	return G.hermitian()*(G*G.hermitian()).inverse()*(F.hermitian()*F).inverse()*F.hermitian();
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


void Matrix::print(std::ostream&out, int numEnter) {
	out << toString();
	for (int i = 0; i < numEnter; i++)
		cout << endl;
}


Matrix Matrix::buildDdentityMatrix(int t_Row) {
	Matrix res(t_Row, t_Row);
	for (int r = 0; r < t_Row; r++)
		res[r][r] = Complex(1, 0);
	return res;
}


Matrix Matrix::verticalMerge(const Matrix& t_Matrix1, const Matrix& t_Matrix2) {
	if (t_Matrix1.row != t_Matrix2.row) throw Exp("�����������޷��ϲ�");
	Matrix res(t_Matrix1.row, t_Matrix1.col + t_Matrix2.col);
	for (int r = 0; r < t_Matrix1.row; r++) {
		for (int c = 0; c < t_Matrix1.col; c++) {
			res[r][c] = t_Matrix1[r][c];
		}
	}
	for (int r = 0; r < t_Matrix2.row; r++) {
		for (int c = 0; c < t_Matrix2.col; c++) {
			res[r][c + t_Matrix1.col] = t_Matrix2[r][c];
		}
	}
	return res;
}


std::pair<Matrix, Matrix> Matrix::verticalSplit(const Matrix& t_Matrix,int leftCol,int rightCol) {
	if (t_Matrix.col != leftCol+ rightCol) throw Exp("�þ����޷����ѳ�ָ��ά��");
	Matrix left(t_Matrix.row, leftCol), right(t_Matrix.row, rightCol);
	for (int r = 0; r < left.row; r++) {
		for (int c = 0; c < left.col; c++) {
			left[r][c] = t_Matrix[r][c];
		}
	}

	for (int r = 0; r < right.row; r++) {
		for (int c = 0; c < right.col; c++) {
			right[r][c] = t_Matrix[r][c + left.col];
		}
	}
	return pair<Matrix, Matrix>(left, right);
}


std::pair<Matrix, Matrix> Matrix::horizonSplit(const Matrix& t_Matrix, int upRow, int downRow) {
	if (t_Matrix.row != upRow + downRow) throw Exp("�þ����޷����ѳ�ָ��ά��");
	Matrix up(upRow, t_Matrix.col), down(downRow, t_Matrix.col);
	for (int r = 0; r < up.row; r++) {
		for (int c = 0; c < up.col; c++) {
			up[r][c] = t_Matrix[r][c];
		}
	}

	for (int r = 0; r < down.row; r++) {
		for (int c = 0; c < down.col; c++) {
			down[r][c] = t_Matrix[r + up.row][c];
		}
	}
	return pair<Matrix, Matrix>(up, down);
}

Matrix Matrix::eye(const int dim) {
	if (dim < 1) throw Exp("��λ��ά������Ϊ1");
	Matrix res(dim, dim);
	for (int i = 0; i < dim; i++) {
		res[i][i] = 1;
	}
	return res;
}


//��Ŀȡ�������
Matrix operator-(const Matrix t_Matrix) {
	Matrix res(t_Matrix.row, t_Matrix.col);
	for (int r = 0; r < t_Matrix.row; r++) {
		res[r] = -t_Matrix[r];
	}
	return res;
}


//����������
Matrix operator+(const Matrix t_Matrix1, const Matrix t_Matrix2) {
	if (t_Matrix1.row != t_Matrix2.row || t_Matrix1.col != t_Matrix2.col) throw Exp("����ά�Ȳ�ͬ���޷����");
	Matrix res(t_Matrix1.row, t_Matrix1.col);
	for (int r = 0; r < t_Matrix1.row; r++) {
		for (int c = 0; c < t_Matrix1.col; c++) {
			res[r][c] = t_Matrix1[r][c] + t_Matrix2[r][c];
		}
	}
	return res;
}


Matrix operator-(const Matrix t_Matrix1, const Matrix t_Matrix2) {
	if (t_Matrix1.row != t_Matrix2.row || t_Matrix1.col != t_Matrix2.col) throw Exp("����ά�Ȳ�ͬ���޷����");
	Matrix res(t_Matrix1.row, t_Matrix1.col);
	for (int r = 0; r < t_Matrix1.row; r++) {
		for (int c = 0; c < t_Matrix1.col; c++) {
			res[r][c] = t_Matrix1[r][c] - t_Matrix2[r][c];
		}
	}
	return res;
}


Matrix operator*(const Matrix t_Matrix1, const Matrix t_Matrix2) {
	if (t_Matrix1.col != t_Matrix2.row) throw Exp("����ά�Ȳ�ƥ�䣬�޷����");

	Matrix res(t_Matrix1.row, t_Matrix2.col);

	for (int r = 0; r < t_Matrix1.row; r++) {
		for (int c = 0; c < t_Matrix2.col; c++) {
			//����ÿһ��
			Complex tmp(0, 0);
			for (int idx = 0; idx < t_Matrix1.col; idx++) {
				tmp += t_Matrix1[r][idx] * t_Matrix2[idx][c];
			}
			res[r][c] = tmp;
		}
	}
	return res;
}


//�����븴��������
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
	if (Complex::abs(t_Complex) == 0) throw Exp("����ģֵΪ0���Ƿ���");
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
		t_Matrix1.col != t_Matrix2.col) throw Exp("����ά�Ȳ�ƥ��");
	Matrix res(t_Matrix1.row, t_Matrix1.col);
	for (int r = 0; r < t_Matrix1.row; r++) {
		res[r] = elementProduct(t_Matrix1[r], t_Matrix2[r]);
	}
	return res;
}
Matrix elementDivide(const Matrix t_Matrix1, const Matrix t_Matrix2) {
	if (t_Matrix1.row != t_Matrix2.row ||
		t_Matrix1.col != t_Matrix2.col) throw Exp("����ά�Ȳ�ƥ��");
	Matrix res(t_Matrix1.row, t_Matrix1.col);
	for (int r = 0; r < t_Matrix1.row; r++) {
		res[r] = elementDivide(t_Matrix1[r], t_Matrix2[r]);
	}
	return res;
}