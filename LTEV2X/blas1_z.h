#pragma once
#include<complex>
double dznrm2(int n, std::complex <double> x[], int incx);
double dzasum(int n, std::complex <double> x[], int incx);
int i4_max(int i1, int i2);
int i4_min(int i1, int i2);
int izamax(int n, std::complex <double> x[], int incx);
bool lsame(char ca, char cb);
double r8_abs(double x);
double r8_max(double x, double y);
double r8_sign(double x);
void xerbla(char *srname, int info);
double zabs1(std::complex <double> z);
double zabs2(std::complex <double> z);
void zaxpy(int n, std::complex <double> ca, std::complex <double> cx[],
	int incx, std::complex <double> cy[], int incy);
void zcopy(int n, std::complex <double> cx[], int incx, std::complex <double> cy[],
	int incy);
std::complex <double> zdotc(int n, std::complex <double> cx[], int incx,
	std::complex <double> cy[], int incy);
std::complex <double> zdotu(int n, std::complex <double> cx[], int incx,
	std::complex <double> cy[], int incy);
void zdrot(int n, std::complex <double> cx[], int incx, std::complex <double> cy[],
	int incy, double c, double s);
void zdscal(int n, double sa, std::complex <double> cx[], int incx);
double zmach(int job);
void zrotg(std::complex <double> *ca, std::complex <double> cb, double *c,
	std::complex <double> *s);
void zscal(int n, std::complex <double> ca, std::complex <double> cx[], int incx);
std::complex <double> zsign1(std::complex <double> z1, std::complex <double> z2);
std::complex <double> zsign2(std::complex <double> z1, std::complex <double> z2);
void zswap(int n, std::complex <double> cx[], int incx, std::complex <double> cy[],
	int incy);
