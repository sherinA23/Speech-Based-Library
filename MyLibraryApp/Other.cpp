#include "StdAfx.h"
#include "Other.h"


Other::Other(void)
{
	int i = 0, j = 0, trained_words = 0;

	A = (long double**) calloc(6, sizeof(long double*));
	for(i = 0; i < 6; ++i) {
		A[i] = (long double*) calloc(6, sizeof(long double));
	}

	B = (long double**) calloc(6, sizeof(long double*));
	for(i = 0; i < 6; ++i) {
		B[i] = (long double*) calloc(33, sizeof(long double));
	}

	Pi = (long double*) calloc(6, sizeof(long double));

	O = (int**)malloc(21 * sizeof(int*));
	for(i = 0; i < 21; ++i) {
		O[i] = (int*) calloc(86, sizeof(int));
	}

	Del = (long double**) calloc(86, sizeof(long double*));
	for(i = 0; i < 86; ++i) {
		Del[i] = (long double *) calloc(6, sizeof(long double));
	}

	Q = (int*) calloc(86, sizeof(int));

	Psi = (int**) calloc(86, sizeof(int*));
	for(i = 0; i < 86; i++){
		Psi[i] = (int*) calloc(6, sizeof(int));
	}

	alpha = (long double**) calloc(86, sizeof(long double*));
	for(i = 0; i < 86; i++){
		alpha[i] = (long double*) calloc(6, sizeof(long double));
	}
	
	beta = (long double**) calloc(86, sizeof(long double*));
	for(i = 0; i < 86; i++){
		beta[i] = (long double*) calloc(6, sizeof(long double));
	}

	gamma = (long double**) calloc(86, sizeof(long double*));
	for(i = 0; i < 86; i++){
		gamma[i] = (long double*) calloc(6, sizeof(long double));
	}

	B_bar = (long double**) calloc(6, sizeof(long double*));
	for(i = 0; i < 6; i++){
		B_bar[i] = (long double*) calloc(33, sizeof(long double));
	}

	A_bar = (long double**) calloc(6, sizeof(long double*));
	for(i = 0; i < 6; i++){
		A_bar[i] = (long double*) calloc(6, sizeof(long double));
	}

	Pi_bar = (long double*) calloc(6, sizeof(long double));

	Xi = (long double***) calloc(86, sizeof(long double**));
	for(i = 0; i < 86; i++){
		Xi[i] = (long double**) calloc(6, sizeof(long double*));
	}
	for(i = 0; i < 86; i++){
		for(j = 0; j < 6; j++){
			Xi[i][j] = (long double*) calloc(6, sizeof(long double));
		}
		
	}
	
	A_opt = (long double***) calloc(11, sizeof(long double**));
	for(i = 0; i < 11; i++){
		A_opt[i] = (long double**) calloc(6, sizeof(long double*));
	}
	for(i = 0; i < 11; i++){
		for(j = 0; j < 6; j++){
			A_opt[i][j] = (long double*) calloc(6, sizeof(long double));
		}
		
	}

	A_avg = (long double***) calloc(11, sizeof(long double**));
	for(i = 0; i < 11; i++){
		A_avg[i] = (long double**) calloc(6, sizeof(long double*));
	}
	for(i = 0; i < 11; i++){
		for(j = 0; j < 6; j++){
			A_avg[i][j] = (long double*) calloc(6, sizeof(long double));
		}
		
	}

	B_opt = (long double***) calloc(11, sizeof(long double**));
	for(i = 0; i < 11; i++){
		B_opt[i] = (long double**) calloc(6, sizeof(long double*));
	}
	for(i = 0; i < 11; i++){
		for(j = 0; j < 6; j++){
			B_opt[i][j] = (long double*) calloc(86, sizeof(long double));
		}
		
	}

	B_avg = (long double***) calloc(11, sizeof(long double**));
	for(i = 0; i < 11; i++){
		B_avg[i] = (long double**) calloc(6, sizeof(long double*));
	}
	for(i = 0; i < 11; i++){
		for(j = 0; j < 6; j++){
			B_avg[i][j] = (long double*) calloc(33, sizeof(long double));
		}
		
	}

	Pi_opt = (long double**) calloc(11, sizeof(long double*));
	for(i = 0; i < 11; i++){
		Pi_opt[i] = (long double*) calloc(6, sizeof(long double));
	}

	Pi_avg = (long double**) calloc(11, sizeof(long double*));
	for(i = 0; i < 11; i++){
		Pi_avg[i] = (long double*) calloc(6, sizeof(long double));
	}

	arr = (float*) calloc(45000, sizeof(float));
}

