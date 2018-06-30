//------------------------------------------------------------
// �W���I�ȍ����i���̃A���S���Y�������������v���O����
//------------------------------------------------------------
//------------------------------------------------------------
//�uerror C2065: 'M_PI' : ��`����Ă��Ȃ����ʎq�ł��v�ւ̑Ή�
//�@VisualStudio�̏ꍇ�͈ȉ��̃R�����g���O���ăR���p�C��
//------------------------------------------------------------
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <random>

using namespace std;

// �ȉ��̓����Z���k�c�C�X�^�[�Ŏg�p����
#include "MT.h"
//------------------------------------------------------------
// �����Z���k�c�C�X�^�[�Ŏg�p����֐��̊T��
// genrand_int32() //�����Ȃ�32�r�b�g������
// genrand_int31() //�����Ȃ�31�r�b�g������
// genrand_real1() //��l������[0,1] (32�r�b�g���x)
// genrand_real2() //��l������[0,1) (32�r�b�g���x)
// genrand_real3() //��l������(0,1) (32�r�b�g���x)
// genrand_res53() //��l������[0,1) (53�r�b�g���x)
//------------------------------------------------------------
//------------------------------------------------------------
// VisualStudio�̏ꍇ�͈ȉ��̃R�����g���O���ăR���p�C��
//------------------------------------------------------------
#define _CRT_SECURE_NO_DEPRECATE
#pragma warning(disable:4996)
//------------------------------------------------------------
// �ݒ�p�����[�^ �������ɂ��̕���������������
//------------------------------------------------------------
#define MaxPsize		50					//�ő�̐�
#define MaxGsize		10					//�ő原�����i���̎������j
#define FuncNo			3					//�œK�����̎��
#define RANGE			5.12				//�œK�����̒�`��
#define MRATE			0.9				//�ˑR�ψٗ�
#define CRATE			0.05				//������
#define MaxGeneration	100			//�ő�J��Ԃ���
#define DEalgorithmNo	5				//DE�̃A���S���Y��
#define ExTime			20					//���s��
#define Terminate		1.0e-6			//�I������
//------------------------------------------------------------
double cVect[MaxPsize][MaxGsize];			//����T�̌́i�x�N�g���j
double cFitness[MaxPsize];							//����T�̌̂̕]���l
double pBestVector[MaxPsize][MaxGsize];	//����T�ɂ�����e�̂̍ŗǉ��̗���
double pBestFitness[MaxPsize];				//����T�ɂ�����e�̂̍ŗǉ�]���l�̗���
double gBestVector[MaxGsize];				//����T�ɂ�����W�c�S�̂ł̍ŗǉ��̗���
double gBestFitness;							//����T�ɂ�����W�c�S�̂ł̍ŗǉ�]���l�̗���
double pVect1[MaxGsize];					//�e�x�N�g���P
double pVect2[MaxGsize];					//�e�x�N�g���Q
double pVect3[MaxGsize];					//�e�x�N�g���R
double nVect[MaxPsize][MaxGsize];	//����T+1�̌́i�x�N�g���j
double nFitness[MaxPsize];				//����T+1�̌̂̕]���l
double gBestHistory[ExTime][MaxGeneration];	//�e���s�ɂ�����gBest�̗���
double pDiversity[ExTime][MaxGeneration];	//�e���s�ɂ�����W�c�̑��l��
double Mrate[MaxPsize],cMrate=0.0;
double Crate[MaxPsize],cCrate=0.0;
double C = 0.1;	//JADE �̃p�����[�^

int gTable[ExTime];					//�œK�𔭌����̐��㐔
int sRate[ExTime];						//�œK���̔�����
double gBestTable[ExTime];		//�I�����_�ł�gBest�̒l
//------------------------------------------------------------
//------------------------------------------------------------
// [min, max]�̈�l����
//------------------------------------------------------------
double uniform(double min, double max) {
	return min + (max - min)*genrand_real1();
}
//------------------------------------------------------------
// �e�X�g�֐�
// F1 Sphere�֐�
// [-5.12, +5.12] x=(0,..,0) F1=0 
//------------------------------------------------------------
double Sphere(double *x) {
	int i;
	double sum = 0.0;
	for (i = 0; i<MaxGsize; i++) {
		sum += x[i] * x[i];
	}
	return sum;
}
//------------------------------------------------------------
// F2 Rosenbrock�֐� Chain�^
// [-2.048, +2.048] x=(1,..,1) F2=0 
//------------------------------------------------------------
double Rosenbrock(double *x) {
	int i;
	double sum = 0.0;
	/*
	for (i = 0; i<MaxGsize - 1; i++){
	sum += 100 * (x[i + 1] - x[i] * x[i])*(x[i + 1] - x[i] * x[i]) + (x[i] - 1.0)*(x[i] - 1.0);
	}
	*/
	// UNDX�̕����̎��ɓ���
	// Rosenbrock�֐� Star�^
	for (i = 2; i<MaxGsize; i++) {
		sum += (100 * (x[0] - x[i] * x[i])*(x[0] - x[i] * x[i]) + (x[1] - 1.0)*(x[1] - 1.0));
	}
	return sum;
}
//------------------------------------------------------------
// F3 Rastrigin�֐�
//------------------------------------------------------------
double Rastrigin(double *x) {
	int i;
	double sum1 = 0, sum2 = 0;
	for (i = 0; i<MaxGsize; i++) {
		sum1 += x[i] * x[i] - 10 * cos(2 * M_PI*x[i]);
	}
	return (10 * MaxGsize + sum1);
}
//------------------------------------------------------------
// F4 Griewank�֐�
//------------------------------------------------------------
double Griewank(double *x) {
	int i;
	double sum1 = 0.0, sum2 = 1.0;
	for (i = 0; i<MaxGsize; i++) {
		sum1 += x[i] * x[i];
		sum2 *= cos(x[i] / sqrt((i + 1)*1.0));
	}
	return ((sum1 / 4000) - sum2 + 1);
}
//------------------------------------------------------------
// F5 Ackley�֐�
//------------------------------------------------------------
double Ackley(double *x) {
	int i;
	double sum1, sum2;
	for (i = 0, sum1 = 0.0; i<MaxGsize; i++)sum1 += x[i] * x[i];
	for (i = 0, sum2 = 0.0; i<MaxGsize; i++)sum2 += cos(2.0*M_PI*MaxGsize);
	return (20.0 + M_E - 20.0 * exp(-0.2*sqrt(sum1 / MaxGsize)) - exp(sum2 / MaxGsize));
}
//------------------------------------------------------------
// F6 Schwefel�֐�
//------------------------------------------------------------
double Schwefel(double *x) {
	int i;
	double sum;
	for (i = 0, sum = 0.0; i<MaxGsize; i++) {
		sum += x[i] * sin(sqrt(fabs(x[i])));
	}
	return (sum + 418.9828872724338 * MaxGsize);
}
//------------------------------------------------------------
// F7 Ridge�֐�
//------------------------------------------------------------
double Ridge(double *x) {
	int i, j;
	double sum1, sum2;
	for (i = 0, sum1 = 0; i<MaxGsize; i++) {
		for (j = 0, sum2 = 0; j <= i; j++) {
			sum2 += x[j];
		}
		sum1 += sum2 * sum2;
	}
	return sum1;
}
//------------------------------------------------------------
// F8 Bohachevsky�֐�
//------------------------------------------------------------
double Bohachevsky(double *x) {
	int i;
	double sum;
	for (i = 0, sum = 0; i<MaxGsize - 1; i++) {
		sum += x[i] * x[i] + 2 * x[i + 1] * x[i + 1] - 0.3*cos(3 * M_PI*x[i]) - 0.4*cos(4 * M_PI*x[i + 1]) + 0.7;
	}
	return sum;
}
//------------------------------------------------------------
// F9 Schaffer�֐�
//------------------------------------------------------------
double Schaffer(double *x) {
	int i;
	double tp1 = 0, tp2 = 0, tp3 = 0, tp4 = 0, tp5 = 0;
	for (i = 0, tp5 = 0; i<MaxGsize - 1; i++) {
		tp1 = x[i] * x[i] + x[i + 1] * x[i + 1];
		tp2 = pow(tp1, 0.25);
		tp3 = pow(tp1, 0.1);
		tp4 = sin(50 * tp3) * sin(50 * tp3);
		tp5 += tp2 * (tp4 + 1.0);
	}
	return tp5;
}
//------------------------------------------------------------
// F10 Ellipsoid�֐�
//------------------------------------------------------------
double Ellipsoid(double *x) {
	int i;
	double sum;
	for (i = 0, sum = 0; i<MaxGsize; i++) {
		sum += (pow(1000, (double)i / (double)(MaxGsize - 1)) * x[i]) * (pow(1000, (double)i / (double)(MaxGsize - 1)) * x[i]);
	}
	return sum;
}
//------------------------------------------------------------
// F11 k-tablet�֐�
//------------------------------------------------------------
double K_Tablet(double *x) {
	int i;
	double sum1, sum2;
	for (i = 0, sum1 = 0; i<(int)MaxGsize / 2; i++) {
		sum1 += x[i] * x[i];
	}
	for (i = (int)MaxGsize / 2 + 1, sum2 = 0; i<MaxGsize; i++) {
		sum2 += (100 * x[i]) * (100 * x[i]);
	}
	return (sum1 + sum2);
}
//------------------------------------------------------------
// F12 Shifted-Rastrigin�֐�
//------------------------------------------------------------
double Shifted_Rastrigin(double *x) {
	int i;
	double sum1 = 0.0, sum2 = 0.0;
	for (i = 0; i<MaxGsize; i++) sum1 += (x[i] - 1) * (x[i] - 1) - 10 * cos(2 * M_PI*(x[i] - 1));
	sum2 = 10 * MaxGsize + sum1;
	return sum2;
}
//------------------------------------------------------------
// F13 Cigar�֐�
//------------------------------------------------------------
double Cigar(double *x) {
	int i;
	double sum;
	for (i = 1, sum = 0; i<MaxGsize; i++) {
		sum += (1000 * x[i]) * (1000 * x[i]);
	}
	return (x[0] * x[0] + sum);
}
//------------------------------------------------------------
// F13 2n-minima�֐�
//------------------------------------------------------------
double N2_Minima(double *x) {
	int i;
	double sum;
	for (i = 0, sum = 0; i<MaxGsize; i++) {
		sum += (pow(x[i], 4) - 16 * pow(x[i], 2) + 5 * x[i]);
	}
	return (sum / 2);
}
//------------------------------------------------------------
// �ړI�֐��l���v�Z
//------------------------------------------------------------
double Calc_Objective_Function(double *x)
{
	if (FuncNo == 1)return  Sphere(x);
	else if (FuncNo == 2)return  Rosenbrock(x);
	else if (FuncNo == 3)return  Rastrigin(x);
	else if (FuncNo == 4)return  Griewank(x);
	else if (FuncNo == 5)return  Ackley(x);
	else if (FuncNo == 6)return  Schwefel(x);
	else if (FuncNo == 7)return  Ridge(x);
	else if (FuncNo == 8)return  Bohachevsky(x);
	else if (FuncNo == 9)return  Schaffer(x);
	else if (FuncNo == 10)return Ellipsoid(x);
	else if (FuncNo == 11)return K_Tablet(x);
	else if (FuncNo == 12)return Shifted_Rastrigin(x);
	else if (FuncNo == 13)return Cigar(x);
	else if (FuncNo == 14)return N2_Minima(x);
	else exit(0);
}
//------------------------------------------------------------
//�����̌Q�̐���
//------------------------------------------------------------
void Init_Vector(void)
{
	int i, j;		//�J��Ԃ��p�ϐ�
	double r;		//��`��p�ϐ�
	r = RANGE;
	for (i = 0; i<MaxPsize; i++) {
		for (j = 0; j<MaxGsize; j++) {
			cVect[i][j] = RANGE*(genrand_real1() * 2 - 1);
		}
	}
	//�����x�N�g����pBestVector�ɕۑ�����
	for (i = 0; i<MaxPsize; i++) {
		for (j = 0; j<MaxGsize; j++) {
			pBestVector[i][j] = cVect[i][j];
		}
	}
}
//------------------------------------------------------------
// �����W�c�̕]���i�ړI�֐��l��K���x�Ƃ��ė��p�j
//------------------------------------------------------------
void Evaluate_Init_Vector(void) {
	int i;
	for (i = 0; i<MaxPsize; i++) {
		cFitness[i] = Calc_Objective_Function(cVect[i]);
		//�����l������pBest�Ƃ��ĕۑ�
		pBestFitness[i] = cFitness[i];
	}
}
//------------------------------------------------------------
//�V�K�x�N�g���̐����̂��߂̐e�x�N�g���̑I��
//------------------------------------------------------------
void Select_pVector(int pop1)
{
	register int i;
	int pop2, pop3, pop4;
	do {
		pop2 = (int)MaxPsize*genrand_real1();
		pop3 = (int)MaxPsize*genrand_real1();
		pop4 = (int)MaxPsize*genrand_real1();
	} while (pop1 == pop2 || pop1 == pop3 || pop1 == pop4 || pop2 == pop3 || pop2 == pop4 || pop3 == pop4);
	for (i = 0; i<MaxGsize; i++)pVect1[i] = cVect[pop2][i];
	for (i = 0; i<MaxGsize; i++)pVect2[i] = cVect[pop3][i];
	for (i = 0; i<MaxGsize; i++)pVect3[i] = cVect[pop4][i];
}
//------------------------------------------------------------
//DE�̑���
//------------------------------------------------------------
void DE_Operation(int pop1)
{
	register int i;
	int N = 0, L = 0;
	//DE/rand/1/exp
	if (DEalgorithmNo == 1) {
		for (i = 0; i<MaxGsize; i++) nVect[pop1][i] = cVect[pop1][i];
		N = (int)(genrand_real1()*MaxGsize);
		L = 0;
		do {
			nVect[pop1][N] = pVect1[N] + MRATE * (pVect2[N] - pVect3[N]);
			if (nVect[pop1][N] < -RANGE) nVect[pop1][N] = pVect1[N] + genrand_real1() * (-RANGE - pVect1[N]);
			if (nVect[pop1][N] >  RANGE) nVect[pop1][N] = pVect1[N] + genrand_real1() * (RANGE - pVect1[N]);
			N = (N + 1) % MaxGsize;
			L++;
		} while (genrand_real1() < CRATE && L < MaxGsize);
	}
	//DE/best/1/exp
	else if (DEalgorithmNo == 2) {
		for (i = 0; i<MaxGsize; i++) nVect[pop1][i] = cVect[pop1][i];
		N = (int)(genrand_real1()*MaxGsize);
		L = 0;
		do {
			nVect[pop1][N] = gBestVector[N] + MRATE * (pVect2[N] - pVect3[N]);
			if (nVect[pop1][N] < -RANGE) nVect[pop1][N] = pVect1[N] + genrand_real1() * (-RANGE - pVect1[N]);
			if (nVect[pop1][N] >  RANGE) nVect[pop1][N] = pVect1[N] + genrand_real1() * (RANGE - pVect1[N]);
			N = (N + 1) % MaxGsize;
			L++;
		} while (genrand_real1() < CRATE && L < MaxGsize);
	}
	//DE/rand/1/bin
	else if (DEalgorithmNo == 3) {
		for (i = 0; i<MaxGsize; i++) nVect[pop1][i] = cVect[pop1][i];
		N = (int)(genrand_real1()*MaxGsize);
		for (L = 0; L<MaxGsize; L++) {
			if (L == 0 || genrand_real1() < CRATE) {
				nVect[pop1][N] = pVect1[N] + MRATE * (pVect2[N] - pVect3[N]);
				if (nVect[pop1][N] < -RANGE) nVect[pop1][N] = pVect1[N] + genrand_real1() * (-RANGE - pVect1[N]);
				if (nVect[pop1][N] >  RANGE) nVect[pop1][N] = pVect1[N] + genrand_real1() * (RANGE - pVect1[N]);
			}
			else {
				nVect[pop1][N] = cVect[pop1][N];
			}
			N = (N + 1) % MaxGsize;
		}
	}
	//DE/best/1/bin
	else if (DEalgorithmNo == 4) {
		for (i = 0; i<MaxGsize; i++) nVect[pop1][i] = cVect[pop1][i];
		N = (int)(genrand_real1()*MaxGsize);
		for (L = 0; L<MaxGsize; L++) {
			if (L == 0 || genrand_real1() < CRATE) {
				nVect[pop1][N] = gBestVector[N] + MRATE * (pVect2[N] - pVect3[N]);
				if (nVect[pop1][N] < -RANGE) nVect[pop1][N] = pVect1[N] + genrand_real1() * (-RANGE - pVect1[N]);
				if (nVect[pop1][N] >  RANGE) nVect[pop1][N] = pVect1[N] + genrand_real1() * (RANGE - pVect1[N]);
			}
			else {
				nVect[pop1][N] = cVect[pop1][N];
			}
			N = (N + 1) % MaxGsize;
		}
	}
	//JADE	DE/curreny-to-pbest/1
	else if (DEalgorithmNo == 5) {
		for (i = 0; i<MaxGsize; i++) nVect[pop1][i] = cVect[pop1][i];
		N = (int)(genrand_real1()*MaxGsize);
		for (L = 0; L<MaxGsize; L++) {
			if (L == 0 || genrand_real1() < CRATE) {
				nVect[pop1][N] =	pVect1[pop1]+Mrate[pop1]*(gBestVector[N]-pVect1[N]) + Mrate[pop1] * (pVect1[pop1] - pVect2[pop1]);	//�ύX��
				if (nVect[pop1][N] < -RANGE) nVect[pop1][N] = pVect1[N] + genrand_real1() * (-RANGE - pVect1[N]);
				if (nVect[pop1][N] >  RANGE) nVect[pop1][N] = pVect1[N] + genrand_real1() * (RANGE - pVect1[N]);
			}else {
				nVect[pop1][N] = cVect[pop1][N];
			}
			N = (N + 1) % MaxGsize;
		}
	}
	else exit(0);
}
//------------------------------------------------------------
//�V�����x�N�g���̕]��
//------------------------------------------------------------
void Evaluate_New_Vector(int pop1)
{
	nFitness[pop1] = Calc_Objective_Function(nVect[pop1]);

}
//------------------------------------------------------------
//�x�N�g���̔�r
//------------------------------------------------------------
void Compare_Vector(void)
{
	int i, j;				//�J�Ԃ��p�ϐ�.
	for (i = 0; i<MaxPsize; i++) {
		//�V�����x�N�g�����ǂ���Βu������������s��
		if (nFitness[i] < cFitness[i]) {
			cFitness[i] = nFitness[i];
			for (j = 0; j<MaxGsize; j++)cVect[i][j] = nVect[i][j];
		}
		else continue;
	}
}
//------------------------------------------------------------
//�G���[�g�I��
//------------------------------------------------------------
void Select_Elite_Vector(int itime, int gtime)
{
	int i;					//�J�Ԃ��p�ϐ�.
	int num;				//�Y��
	double best;			//�ꎞ�ۑ��p
	for (i = 0, num = 0, best = cFitness[0]; i<MaxPsize; i++) {
		if (cFitness[i]<best) {
			best = cFitness[i];
			num = i;
		}
	}
	for (i = 0; i<MaxGsize; i++)gBestVector[i] = cVect[num][i];
	gBestFitness = cFitness[num];
	gBestHistory[itime][gtime] = gBestFitness;
	printf("%20.10lf\n", gBestFitness);
}


//JADE �쐬��
void Pameter_Filter(double Sf, double Scr) {
	int i;
	std::random_device seed_gen;//�����̐���
	std::default_random_engine engine(seed_gen());//�[�������̐���
	// �ʒu�ꐔcMrate�A�ړx�ꐔSf�ŕ��z������
	std::cauchy_distribution<> cauchy_data(cMrate, Sf); //Sf��0���߂łȂ��ƃG���[����������
//	std::cauchy_distribution<> cauchy_data(0.0, 1.0);
	// ����cCrate�A�W���΍�Scr^2�ŕ��z������
	std::normal_distribution<> nomal_data(cCrate, Scr*Scr);//Scr��0���傫���Ȃ��ƃG���[����������
//	std::normal_distribution<> nomal_data(0, 1.0);

	for (i = 0; i < MaxPsize; i++) {
	reMrate:
			//Mrate[i] = cauchy_data((unsigned int)genrand_int32);if (Mrate[i]>1 || Mrate[i]<0) goto	reMrate;//�������@���s��
			Mrate[i] = cauchy_data(engine);if (Mrate[i]>1 || Mrate[i]<0) goto	reMrate;//�������@���s��
	reCrate:
		Crate[i] = nomal_data(engine); if (Crate[i]>1 || Crate[i]<0) goto	reCrate;//�������@���s��
	}
}

//�p�����[�^�̍X�V	JADE �쐬��
void New_parameter() {
	int i;
	double Sn = 0, Sf = 1.0, Sf2 = 0, Scr =1.0;
	for (i = 0; i < MaxPsize;i++) {
		if (	fabs(nVect[i][MaxPsize])	>	fabs(cVect[i][MaxPsize])		) {
			Sn += 1;
			Sf2 += Mrate[i]*Mrate[i];
			Scr += Crate[i];
		}
	}
	if (Sn != 0) {
		cMrate = (1 - C)*cMrate + (C*Sf2 / Sf);
		cCrate = (1 - C)*cCrate + ( (C*Scr )/ Sn );
	}
	Pameter_Filter(Sf,Scr);
}




//------------------------------------------------------------
//�t�@�C���o��1�@�ŗǒl�̐���
//------------------------------------------------------------
void Output_To_File1(void)
{
	int i, j;					//�J�Ԃ��p�ϐ�.
	FILE *fp;					//�t�@�C���|�C���^
	char filename[50];			//�t�@�C����
	time_t timer;				//���Ԍv���p
	struct tm *t_st;			//���Ԍv���p
	time(&timer);				//���Ԃ̎擾
	t_st = localtime(&timer);	//���Ԃ̕ϊ�
	sprintf_s(filename, "DE_gBestHistory%04d%02d%02d%02d%02d.txt",
		t_st->tm_year + 1900, t_st->tm_mon + 1,
		t_st->tm_mday, t_st->tm_hour, t_st->tm_min);
	fp = fopen(filename, "a");
	for (i = 0; i<MaxGeneration; i++) {
		for (j = 0; j<ExTime; j++) {
			fprintf(fp, "%20.10lf", gBestHistory[j][i]);
		}
		fprintf(fp, "\n");
	}
	fprintf(fp, "\n");
}
//------------------------------------------------------------
//�W�c�̑��l���̕]��
//------------------------------------------------------------
void Calc_Diversity(int itime, int gtime)
{
	int i, j;					//�J�Ԃ��p�ϐ�.
	double sum[MaxGsize];		//���v
	double ave[MaxGsize];		//����
	double div;					//���U
								//������
	for (i = 0; i<MaxGsize; i++) {
		sum[i] = 0.0;
		ave[i] = 0.0;
	}
	//���v
	for (i = 0; i<MaxPsize; i++) {
		for (j = 0; j<MaxGsize; j++) {
			sum[j] += cVect[i][j];
		}
	}
	//����
	for (i = 0; i<MaxGsize; i++) {
		ave[i] = sum[i] / MaxPsize;
	}
	//���U
	for (i = 0, div = 0.0; i<MaxPsize; i++) {
		for (j = 0; j<MaxGsize; j++) {
			div += (ave[j] - cVect[i][j])*(ave[j] - cVect[i][j]);
		}
	}
	pDiversity[itime][gtime] = div / MaxPsize;
}
//------------------------------------------------------------
//�t�@�C���o��2�@���U�l�̐���
//------------------------------------------------------------
void Output_To_File2(void)
{
	int i, j;					//�J�Ԃ��p�ϐ�.
	FILE *fp;					//�t�@�C���|�C���^
	char filename[50];			//�t�@�C����
	time_t timer;				//���Ԍv���p
	struct tm *t_st;			//���Ԍv���p
	time(&timer);				//���Ԃ̎擾
	t_st = localtime(&timer);	//���Ԃ̕ϊ�
	sprintf_s(filename, "DE_pDiversity%04d%02d%02d%02d%02d.txt",
		t_st->tm_year + 1900, t_st->tm_mon + 1,
		t_st->tm_mday, t_st->tm_hour, t_st->tm_min);
	fp = fopen(filename, "a");
	for (i = 0; i<MaxGeneration; i++) {
		for (j = 0; j<ExTime; j++) {
			fprintf(fp, "%20.10lf", pDiversity[j][i]);
		}
		fprintf(fp, "\n");
	}
	fprintf(fp, "\n");
}
//------------------------------------------------------------
//�t�@�C���o��3�@�œK�𔭌�����C������
//------------------------------------------------------------
void Output_To_File3(void)
{
	register int i;
	FILE *fp;
	char filename[50];
	time_t timer;
	struct tm *t_st;
	time(&timer);
	t_st = localtime(&timer);
	sprintf_s(filename, "DE_sRate_gTable%04d%02d%02d%02d%02d.txt",
		t_st->tm_year + 1900, t_st->tm_mon + 1,
		t_st->tm_mday, t_st->tm_hour, t_st->tm_min);
	fp = fopen(filename, "a");
	for (i = 0; i<ExTime; i++) {
		fprintf(fp, "%6d %d\n", gTable[i], sRate[i]);
	}
}
//------------------------------------------------------------
//���C���֐�
//------------------------------------------------------------
int main(void)
{
	int episode;
	int iteration;
	int pop;
	init_genrand((unsigned)time(NULL));	//MT�̏�����
	for (iteration = 0; iteration<ExTime; iteration++) {
		episode = 0;
		Init_Vector();
		Evaluate_Init_Vector();
		while (episode<MaxGeneration) {
			Select_Elite_Vector(iteration, episode);
			Calc_Diversity(iteration, episode);
			for (pop = 0; pop<MaxPsize; pop++) {
				Select_pVector(pop);
				DE_Operation(pop);
				New_parameter();
				Evaluate_New_Vector(pop);
			}
			Compare_Vector();
			episode++;
		}
		gTable[iteration] = episode;
		if (gBestFitness<Terminate)sRate[iteration] = 1;
		else sRate[iteration] = 0;
		gBestTable[iteration] = gBestFitness;
	}
	Output_To_File1();
	Output_To_File2();
	Output_To_File3();
	getchar();
	return 0;
}