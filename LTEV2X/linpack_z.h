#pragma once
#pragma once
#include<complex>

#define max(a,b) (((a) > (b)) ? (a) : (b))

void drotg(double *sa, double *sb, double *c, double *s);
int zchdc(std::complex <double> a[], int lda, int p, int ipvt[], int job);
int zchdd(std::complex <double> r[], int ldr, int p, std::complex <double> x[],
	std::complex <double> z[], int ldz, int nz, std::complex <double> y[], double rho[],
	double c[], std::complex <double> s[]);
void zchex(std::complex <double> r[], int ldr, int p, int k, int l,
	std::complex <double> z[], int ldz, int nz, double c[], std::complex <double> s[], int job);
void zchud(std::complex <double> r[], int ldr, int p, std::complex <double> x[],
	std::complex <double> z[], int ldz, int nz, std::complex <double> y[], double rho[],
	double c[], std::complex <double> s[]);
double zgbco(std::complex <double> abd[], int lda, int n, int ml, int mu, int ipvt[]);
void zgbdi(std::complex <double> abd[], int lda, int n, int ml, int mu, int ipvt[],
	std::complex <double> det[2]);
int zgbfa(std::complex <double> abd[], int lda, int n, int ml, int mu, int ipvt[]);
void zgbsl(std::complex <double> abd[], int lda, int n, int ml, int mu,
	int ipvt[], std::complex <double> b[], int job);
double zgeco(std::complex <double> a[], int lda, int n, int ipvt[]);
void zgedi(std::complex <double> a[], int lda, int n, int ipvt[],
	std::complex <double> det[2], int job);
int zgefa(std::complex <double> a[], int lda, int n, int ipvt[]);
void zgesl(std::complex <double> a[], int lda, int n, int ipvt[],
	std::complex <double> b[], int job);
int zgtsl(int n, std::complex <double> c[], std::complex <double> d[],
	std::complex <double> e[], std::complex <double> b[]);
double zhico(std::complex <double> a[], int lda, int n, int ipvt[]);
void zhidi(std::complex <double> a[], int lda, int n, int ipvt[], double det[2],
	int inert[3], int job);
int zhifa(std::complex <double> a[], int lda, int n, int ipvt[]);
void zhisl(std::complex <double> a[], int lda, int n, int ipvt[],
	std::complex <double> b[]);
double zhpco(std::complex <double> ap[], int n, int ipvt[]);
void zhpdi(std::complex <double> ap[], int n, int ipvt[], double det[2],
	int inert[3], int job);
int zhpfa(std::complex <double> ap[], int n, int ipvt[]);
void zhpsl(std::complex <double> ap[], int n, int ipvt[], std::complex <double> b[]);
double zpbco(std::complex <double> abd[], int lda, int n, int m, int *info);
void zpbdi(std::complex <double> abd[], int lda, int n, int m, double det[2]);
int zpbfa(std::complex <double> abd[], int lda, int n, int m);
void zpbsl(std::complex <double> abd[], int lda, int n, int m, std::complex <double> b[]);
double zpoco(std::complex <double> a[], int lda, int n, int *info);
void zpodi(std::complex <double> a[], int lda, int n, double det[2], int job);
int zpofa(std::complex <double> a[], int lda, int n);
void zposl(std::complex <double> a[], int lda, int n, std::complex <double> b[]);
double zppco(std::complex <double> ap[], int n, int *info);
void zppdi(std::complex <double> ap[], int n, double det[2], int job);
int zppfa(std::complex <double> ap[], int n);
void zppsl(std::complex <double> ap[], int n, std::complex <double> b[]);
void zptsl(int n, std::complex <double> d[], std::complex <double> e[],
	std::complex <double> b[]);
void zqrdc(std::complex <double> x[], int ldx, int n, int p,
	std::complex <double> qraux[], int ipvt[], int job);
int zqrsl(std::complex <double> x[], int ldx, int n, int k, std::complex <double> qraux[],
	std::complex <double> y[], std::complex <double> qy[], std::complex <double> qty[],
	std::complex <double> b[], std::complex <double> rsd[], std::complex <double> xb[], int job);
double zsico(std::complex <double> a[], int lda, int n, int ipvt[]);
void zsidi(std::complex <double> a[], int lda, int n, int ipvt[],
	std::complex <double> det[2], int job);
int zsifa(std::complex <double> a[], int lda, int n, int ipvt[]);
void zsisl(std::complex <double> a[], int lda, int n, int ipvt[], std::complex <double> b[]);
double zspco(std::complex <double> ap[], int n, int ipvt[]);
void zspdi(std::complex <double> ap[], int n, int ipvt[], std::complex <double> det[2],
	int job);
int zspfa(std::complex <double> ap[], int n, int ipvt[]);
void zspsl(std::complex <double> ap[], int n, int ipvt[], std::complex <double> b[]);
int zsvdc(std::complex <double> x[], int ldx, int n, int p,
	std::complex <double> s[], std::complex <double> e[], std::complex <double> u[], int ldu,
	std::complex <double> v[], int ldv, int job);
double ztrco(std::complex <double> t[], int ldt, int n, int job);
int ztrdi(std::complex <double> t[], int ldt, int n, std::complex <double> det[2],
	int job);
int ztrsl(std::complex <double> t[], int ldt, int n, std::complex <double> b[], int job);