#include <math.h>
#include <complex>
#include <iostream>

#include "Numerical.h"
#include "linpack_z.h"

using namespace std;


/*==================================================
*                    Complex
===================================================*/


/*-----Complex构造函数定义-----*/

Complex::Complex() :m_Real(0), m_Imag(0) {}

Complex::Complex(double r) : m_Real(r), m_Imag(0) {}

Complex::Complex(double r, double i) : m_Real(r), m_Imag(i) {}

Complex::Complex(const Complex& a) : m_Real(a.m_Real), m_Imag(a.m_Imag) {}

Complex::~Complex() {}


/*-----Complex成员函数重载运算符定义-----*/

Complex& Complex::operator =(const double& a) {
	m_Real = a;
	m_Imag = 0;
	return *this;
}

Complex& Complex::operator =(const Complex& c) {
	m_Real = c.m_Real;
	m_Imag = c.m_Imag;
	return *this;
}

Complex& Complex::operator +=(const Complex& c) {
	m_Real += c.m_Real;
	m_Imag += c.m_Imag;
	return *this;
}

Complex& Complex::operator -=(const Complex& c) {
	m_Real -= c.m_Real;
	m_Imag -= c.m_Imag;
	return *this;
}


/*-----Complex非成员函数重载运算符定义-----*/

Complex operator +(const Complex& c1, const Complex& c2) {
	return Complex(c1.m_Real + c2.m_Real, c1.m_Imag + c2.m_Imag);
}

Complex operator -(const Complex& c1, const Complex& c2) {
	return Complex(c1.m_Real - c2.m_Real, c1.m_Imag - c2.m_Imag);
}

Complex operator *(const Complex& c1, const Complex& c2) {
	return Complex(c1.m_Real*c2.m_Real - c1.m_Imag*c2.m_Imag, c1.m_Imag*c2.m_Real + c1.m_Real*c2.m_Imag);
}

Complex operator *(const Complex& c1,const double& a) {
	return Complex(c1.m_Real*a, c1.m_Imag*a);
}

Complex operator /(const Complex& c1, const Complex& c2) {
	double denominator = g_abs2(c2);
	if(denominator ==0) throw Exp("Numerical.cpp->operator /(const Complex& c1, const Complex& c2)");
	return Complex(
		(c1.m_Real*c2.m_Real + c1.m_Imag*c2.m_Imag) / denominator,
		(c1.m_Imag*c2.m_Real - c1.m_Real*c2.m_Imag) / denominator
	);
}

Complex operator /(const Complex& c1,const double& a) {
	if (a == 0) throw Exp("Numerical.cpp->operator /(const Complex& c1,const double& a)");
	return Complex(c1.m_Real / a, c1.m_Imag / a);
}


/*-----Complex计算过程中的辅助函数-----*/

double g_abs2(const Complex& c) {
	return c.m_Real*c.m_Real + c.m_Imag*c.m_Imag;
}
double g_abs(const Complex& c) {
	return sqrt(g_abs2(c));
}

Complex g_Conjugate(const Complex& c) {
	return Complex(c.m_Real, -c.m_Imag);
}

double g_Factorial(int K) {
	if (K < 0) throw Exp("Numerical.cpp->Factorial");
	double fac = 1;
	if (K != 0) {
		for (int i = 1; i <= K; i++)
			fac *= i;
	}
	return fac;
}



/*==================================================
*                    matrix
===================================================*/


/*------matrix静态数据成员初始化------*/

Complex Matrix::s_A[64] = { Complex(0,0) };
int Matrix::s_pi[64] = { 0 };
Complex Matrix::s_e[8] = { Complex(0,0) };
Complex Matrix::s_y[8] = { Complex(0,0) };
int Matrix::s_parity = 0;


/*------matrix构造函数定义------*/

void Matrix::copy(const Matrix& m) {
	if (this->m_Cols != m.m_Cols || this->m_Rows != m.m_Rows || this->m_Mem == nullptr)
		throw Exp("Numerical.h->copy");
	memcpy(m_Mem, m.m_Mem, m.m_Cols*m.m_Rows*(sizeof(Complex)));
};

void Matrix::initialize(int r, int c) {
	m_Rows = r;
	m_Cols = c;
	if (r == 0 || c == 0) throw Exp("Numerical.h->init");
	m_Mem = new Complex[m_Rows*m_Cols];
	memset(m_Mem, 0, sizeof(m_Mem));
}

Matrix::Matrix() :m_Rows(0), m_Cols(0), m_Mem(nullptr) {}

Matrix::Matrix(int r, int c) : m_Mem(nullptr) {
	initialize(r, c);
}

Matrix::Matrix(const Matrix& a) {//拷贝构造函数，由于存在动态分配的指针，需要自定义拷贝部分
	m_Rows = a.m_Rows;
	m_Cols = a.m_Cols;
	m_Mem = nullptr;
	if (a.m_Rows != 0 && a.m_Cols != 0) {
		initialize(a.m_Rows, a.m_Cols);
		this->copy(a);
	}
}


/*------matrix资源管理函数定义------*/

void Matrix::clean() {
	if (m_Mem != nullptr)
		delete[] m_Mem;
	m_Mem = nullptr;
	m_Rows = 0;
	m_Cols = 0;
}


Matrix::~Matrix() {
	clean();
};


/*------matrix矩阵功能函数定义------*/

void Matrix::clear() {
	for (int i = 0; i != m_Cols*m_Rows; i++)
		m_Mem[i] = 0.0;
}

Complex Matrix::get(int row, int col) {
	if (row < 0 || row >= m_Rows || col < 0 || col >= m_Cols) throw Exp("Numerical.h->get");
	return m_Mem[row*m_Cols + col];
};

void Matrix::set(Complex a, int row, int col) {
	if (row < 0 || row >= m_Rows || col < 0 || col >= m_Cols) throw Exp("Numerical.cpp->set");
	//assert(row >= 0 && row<rows&&col >= 0 && col<cols);
	this->m_Mem[m_Cols*row + col] = a;
}

Matrix Matrix::diag() {
	Matrix ret(m_Rows, m_Cols);
	int n = m_Rows>m_Cols ? m_Cols : m_Rows;
	for (int i = 0; i != n; ++i)
		ret.m_Mem[i*ret.m_Cols + i] = m_Mem[i*m_Cols + i];
	return ret;
}

void Matrix::eye() {
	if (m_Rows != m_Cols) throw Exp("Numerical.cpp->eye");
	for (int i = 0; i != m_Rows; ++i) {
		for (int j = 0; j != m_Cols; ++j) {
			if (i == j)
				m_Mem[i*m_Cols + j] = 1.0;
			else

				m_Mem[i*m_Cols + j] = 0.0;
		}
	}
}

Matrix Matrix::trans() {//added by xiaojin
	Matrix ret(m_Cols, m_Rows);
	for (int i = 0; i != m_Rows; ++i)
		for (int j = 0; j != m_Cols; ++j)
			ret.m_Mem[j*ret.m_Cols + i] = m_Mem[i*m_Cols + j];
	return ret;
}

Matrix Matrix::hermitian() {
	Matrix ret(m_Cols, m_Rows);
	for (int i = 0; i != m_Rows; ++i) {
		for (int j = 0; j != m_Cols; ++j)
			ret.m_Mem[j*ret.m_Cols + i] = g_Conjugate(m_Mem[i*m_Cols + j]);
	}
	return ret;
}

Complex Matrix::det() {
	if (m_Rows != m_Cols) throw Exp("Numerical.cpp->det()");
	int n = m_Cols;
	if (n*n>64) throw Exp("Numerical.cpp->det()");
	//assert(n*n <= 64);
	memcpy(s_A, m_Mem, sizeof(Complex)*n*n);
	int rst = _LUP_Decomposition(s_A, n, s_pi);
	if (rst == -1)
		return 0.0;
	Complex ret = 1.0;
	for (int i = 0;i != n;i++)
		ret = ret*s_A[i*n + i];
	ret = ret*(s_parity % 2 == 0 ? 1 : -1);
	return ret;
}

Complex Matrix::tr() {
	Complex ret = 0.0;
	for (int i = 0; i != (m_Rows>m_Cols ? m_Cols : m_Rows); i++)
		ret += m_Mem[i*m_Cols + i];
	return ret;
}

Matrix Matrix::inv() {
	int i, j;
	int n = m_Cols;
	Matrix invmat(m_Rows, m_Rows);
	if (n*n > 64) throw Exp("Numerical.cpp->inv");
	//assert(n*n <= 64);
	memcpy(s_A, m_Mem, sizeof(Complex)*n*n);

	int rst = _LUP_Decomposition(s_A, n, s_pi);
	if (rst == -1) throw Exp("Numerical.cpp->inv");
	//assert(rst != -1);
	for (i = 0; i<n; i++)
		s_e[i] = 0.0;
	for (i = 0; i<n; i++) {
		s_e[i] = 1.0;
		_LUP_Solve(s_A, s_pi, s_e, s_y, n);
		for (j = 0; j<n; j++)
			invmat.m_Mem[j*n + i] = s_y[j];
		s_e[i] = 0.0;
	}
	return invmat;
}

Matrix Matrix::inverse() {
	int M = this->m_Rows;
	int N = this->m_Cols;
	int lda = M;
	complex<double> *ca = new complex<double>[M*N];
	int *ipvt = new int[N];
	complex<double> det[2];
	int job = 01;

	Matrix at = this->trans();
	for (int i = 0; i<M*N; i++) {
		ca[i] = complex<double>(at.m_Mem[i].m_Real, at.m_Mem[i].m_Imag);
	}
	zgeco(ca, lda, N, ipvt);	//求ipvt
	zgedi(ca, lda, N, ipvt, det, job); //调用linpack，求逆

	Matrix rtm(M, N);
	for (int i = 0; i<M*N; i++)
		rtm.m_Mem[i] = Complex(ca[i].real(), ca[i].imag());

	delete[] ca;
	delete[] ipvt;

	return rtm.trans();
}

double Matrix::norm_2() //求矩阵范数
{
	double re = 0;
	for (int i = 0;i < (this->m_Cols)*(this->m_Rows);i++)
		re = re + g_abs2(this->m_Mem[i]);
	return re;
}

void Matrix::svd(Matrix* u, Matrix* s, Matrix* v) {
	int M = this->m_Rows;
	int N = this->m_Cols;
	if (u->m_Rows != M || u->m_Cols != M) throw Exp("Numerical.cpp->svd");
	if (v->m_Rows != N || v->m_Cols != N) throw Exp("Numerical.cpp->svd");
	if (s->m_Rows != M || s->m_Cols != N) throw Exp("Numerical.cpp->svd");

	int lda = M;

	complex <double> ca[64 * 3];
	complex <double> cs[64 * 3];
	complex <double> ce[64 * 3];
	complex <double> cu[64 * 3];
	complex <double> cv[64 * 3];

	int ldu = M;
	int ldv = N;
	int job = 11;

	Matrix at = this->trans();
	for (int i = 0; i<M*N;i++) {
		ca[i] = complex<double>(at.m_Mem[i].m_Real, at.m_Mem[i].m_Imag);
	}
	zsvdc(ca, lda, M, N, cs, ce, cu, ldu, cv, ldv, job);


	for (int i = 0;i<M*M;i++)
		u->m_Mem[i] = Complex(cu[i].real(), cu[i].imag());
	Matrix tmpu = *u;
	*u = tmpu.trans();


	for (int i = 0;i<N*N;i++)
		v->m_Mem[i] = Complex(cv[i].real(), cv[i].imag());
	Matrix tmpv = *v;
	*v = tmpv.trans();


	for (int i = 0;i<M*N;i++)
		s->m_Mem[i] = 0.0;
	for (int i = 0;i<(M>N ? N : M);i++)
		s->m_Mem[i*N + i] = Complex(cs[i].real(), cs[i].imag());
}

void Matrix::appendInRow(const Matrix& a) {
	if (m_Rows == 0) {
		initialize(a.m_Rows, a.m_Cols);
		copy(a);
	}
	else {
		if (m_Cols != a.m_Cols) throw Exp("Numerical.cpp->appendInRow");
		Complex* newmem = new Complex[m_Cols*(a.m_Rows + m_Rows)];
		if (newmem == nullptr) throw Exp("Numerical.cpp->appendInRow");
		memcpy(newmem, m_Mem, sizeof(Complex)*m_Rows*m_Cols);
		memcpy(newmem + m_Rows*m_Cols, a.m_Mem, sizeof(Complex)*a.m_Rows*a.m_Cols);
		delete[] m_Mem;
		m_Mem = newmem;
		m_Rows = a.m_Rows + m_Rows;
	}
}

void Matrix::appendInCol(const Matrix& a) {
	if (m_Rows == 0) {
		initialize(a.m_Rows, a.m_Cols);
		copy(a);
	}
	else {
		if (m_Rows != a.m_Rows) throw Exp("Numerical.cpp->appendInCol");
		Complex* newmem = new Complex[m_Rows*(a.m_Cols + m_Cols)];
		if (newmem == nullptr) throw Exp("Numerical.cpp->appendInRow");
		int rst_cols = a.m_Cols + m_Cols;
		for (int i = 0; i != m_Rows; i++) {
			for (int j = 0; j != m_Cols; j++)
				newmem[i*rst_cols + j] = m_Mem[i*m_Cols + j];
			for (int j = m_Cols; j != rst_cols; j++)
				newmem[i*rst_cols + j] = a.m_Mem[i*a.m_Cols + j - m_Cols];
		}
		m_Cols = rst_cols;
		delete[] m_Mem;
		m_Mem = newmem;
	}
}

Matrix Matrix::apart(int rs, int re, int cs, int ce) {
	if (re < rs || rs < 0 || re >= m_Rows) throw Exp("Numerical.cpp->apart");
	//assert(re >= rs&&rs >= 0 && re<rows);
	if (ce < cs || cs < 0 || ce >= m_Cols) throw Exp("Numerical.cpp->apart");
	//assert(ce >= cs&&cs >= 0 && ce<cols);
	Matrix ret(re - rs + 1, ce - cs + 1);
	for (int i = rs; i != re + 1; i++) {
		for (int j = cs; j != ce + 1; j++)
			ret.m_Mem[(i - rs)*(ce - cs + 1) + j - cs] = m_Mem[i*m_Cols + j];
	}
	return ret;
}

int Matrix::_LUP_Decomposition(Complex* A, int n, int* pi) {
	int i, j, k, tmp;
	int kt = 0;
	double p;
	Complex tmpc;
	s_parity = 0;
	for (i = 0; i<n; i++)
		pi[i] = i;
	for (k = 0; k<n; k++) {
		p = 0.0;
		for (i = k; i<n; i++) {
			if (g_abs(A[i*n + k])>p) {
				p = g_abs(A[i*n + k]);
				kt = i;
			}
		}
		if (p == 0.0)
			return -1;
		if (k != kt)
			s_parity++;
		tmp = pi[k];
		pi[k] = pi[kt];
		pi[kt] = tmp;

		for (i = 0; i<n; i++) {
			tmpc = A[k*n + i];
			A[k*n + i] = A[kt*n + i];
			A[kt*n + i] = tmpc;
		}
		for (i = k + 1; i<n; i++) {
			A[i*n + k] = A[i*n + k] / A[k*n + k];
			for (j = k + 1; j<n; j++)
				A[i*n + j] = A[i*n + j] - A[i*n + k] * A[k*n + j];
		}
	}
	return 0;
}

void Matrix::_LUP_Solve(Complex* LU, int* pi, Complex *b, Complex* out, int n) {
	int i, j;
	Complex tmp;
	for (i = 0; i<n; i++) {
		tmp = 0.0;
		for (j = 0; j <= i - 1; j++) {
			tmp += LU[i*n + j] * out[j];
		}
		out[j] = b[pi[i]] - tmp;
	}
	for (i = n - 1; i >= 0; i--) {
		tmp = 0.0;
		for (j = i + 1; j<n; j++)
			tmp += LU[i*n + j] * out[j];
		out[i] = (out[i] - tmp) / LU[i*n + i];
	}
}


/*------matrix成员函数重载运算符定义------*/

Matrix& Matrix::operator =(const Matrix& a){
	clean();
	initialize(a.m_Rows, a.m_Cols);
	copy(a);
	return *this;
}

Matrix&  Matrix::operator += (const Matrix& m) {
	if (m.m_Cols != m_Cols || m.m_Rows != m_Rows) throw Exp("Numerical.cpp->operator+=(const matrix&a)");
	for (int i = 0;i != m_Rows*m_Cols;++i)
		m_Mem[i] = m.m_Mem[i] + m_Mem[i];
	return *this;
}

Matrix& Matrix::operator *=(const double& a) {
	for (int i = 0; i != m_Rows*m_Cols; i++)
		m_Mem[i] = m_Mem[i] * a;
	return *this;
}

Matrix& Matrix::operator /= (double a) {
	for (int i = 0; i != m_Rows*m_Cols; ++i)
		m_Mem[i] = m_Mem[i] / a;
	return *this;
}


/*-----matrix非成员函数重载运算符定义-----*/

Matrix operator + (const Matrix& m1,const Matrix& m2) {
	if (m2.m_Cols != m1.m_Cols || m2.m_Rows != m1.m_Rows) throw Exp("Numerical.cpp->operator+(const matrix&a)");
	Matrix ret(m2.m_Rows, m2.m_Cols);
	for (int i = 0; i != m1.m_Rows*m1.m_Cols; ++i)
		ret.m_Mem[i] = m2.m_Mem[i] + m1.m_Mem[i];
	return ret;
}

Matrix operator - (const Matrix& m1, const Matrix& m2) {
	if (m2.m_Cols != m1.m_Cols || m2.m_Rows != m1.m_Rows) throw Exp("Numerical.cpp->operator-(const matrix&a)");
	Matrix ret(m2.m_Rows, m2.m_Cols);
	for (int i = 0; i != m1.m_Rows*m1.m_Cols; ++i)
		ret.m_Mem[i] = m1.m_Mem[i] - m2.m_Mem[i];
	return ret;
}

Matrix operator *(const Matrix& m1,double a) {
	Matrix ret(m1.m_Rows, m1.m_Cols);
	for (int i = 0;i != m1.m_Rows*m1.m_Cols;i++)
		ret.m_Mem[i] = m1.m_Mem[i] * a;
	return ret;
}

Matrix operator *(const Matrix& m1,const Matrix& m2) {
	if (m1.m_Cols != m2.m_Rows) throw Exp("Numerical.cpp->operator *(const matrix&a)");
	Matrix c(m1.m_Rows, m2.m_Cols);
	int i, j, k;
	int offset_i;
	int offset_j;
	Complex* src = m2.m_Mem;
	Complex* dst = c.m_Mem;
	for (i = 0; i != m1.m_Rows; ++i) {
		offset_i = i*m1.m_Cols;
		offset_j = m2.m_Cols*i;
		for (j = 0; j != m2.m_Cols; ++j) {
			for (k = 0; k != m1.m_Cols; ++k)
				dst[offset_j + j] += m1.m_Mem[offset_i + k] * src[k*m2.m_Cols + j];
		}
	}
	return c;
};

Matrix operator / (const Matrix& m1,const double& a) {
	Matrix ret(m1.m_Rows, m1.m_Cols);
	for (int i = 0; i != m1.m_Rows*m1.m_Cols; ++i)
		ret.m_Mem[i] = m1.m_Mem[i] / a;
	return ret;
}

Matrix operator / (const Matrix& m1,const Complex& a) {
	Matrix ret(m1.m_Rows, m1.m_Cols);
	for (int i = 0; i != m1.m_Rows*m1.m_Cols; ++i)
		ret.m_Mem[i] = m1.m_Mem[i] / a;
	return ret;
}


















