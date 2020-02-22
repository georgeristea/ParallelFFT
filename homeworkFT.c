// RISTEA GEORGE VALENTIN 335CB
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <complex.h>

typedef double complex cplx;

int N;
int P; // nr de threaduri
double* inputValues;
cplx* outputValues;

double PI = atan2(1, 1) * 4;

void* threadFunction(void *args)
{

	int thread_id = *(int*)args;
	int z = ceil((double)N/P);
	int first = thread_id * z;
	int last = (thread_id + 1) * z;
	if (last >= N) {
		last = N;
	}

	int i, k;
	double x;
	cplx result;
	for (k = first; k < last; k++) {
		result = 0;
		for (i = 0; i < N; i++) {
			cplx e = cexp((-2 * PI * I * k * i) / N);
			x = inputValues[i];
			result += x * e;
			outputValues[k] = result;
		}
	}
	return NULL;
}

void readFromFile(char inputFile[1024]) {

	FILE *fp;
	int i;
	fp = fopen(inputFile, "r");
	if (fp == NULL) {
		printf("[R] Cannot open the file \n");
		exit(1);
	}
	if (fscanf(fp, "%d", &N) != 1) {
		printf("problem");
	}

	inputValues = (double*) malloc(N * sizeof(double));
	outputValues = (cplx*) malloc(N * sizeof(cplx));
	if (inputValues == NULL || outputValues == NULL) {
		printf("[ERROR] malloc\n");
		exit(1);
	}
	for (i = 0; i < N; i++) {
		if (fscanf(fp, "%lf", &inputValues[i]) != 1) {
			printf("problem");
		}
	}
	fclose(fp);

}
void writeToFile(char outputFile[1024]) {

	FILE* fp;
	int i;
	fp = fopen(outputFile, "w+");
	if (fp == NULL) {
		printf("[W] Cannot open the file\n");
		exit(1);
	}

	fprintf(fp, "%d\n", N);
	for (i = 0; i < N; i++) {
		fprintf(fp, "%lf %lf\n", creal(outputValues[i]), cimag(outputValues[i]));
	}

	fclose(fp);

}

int main(int argc, char * argv[]) {

	readFromFile(argv[1]);
	P = atoi(argv[3]);

	int i;
	pthread_t tid[P];
	int thread_id[P];

	for(i = 0;i < P; i++)
		thread_id[i] = i;

	for(i = 0; i < P; i++) {
		pthread_create(&(tid[i]), NULL, threadFunction, &(thread_id[i]));
	}

	for(i = 0; i < P; i++) {
		pthread_join(tid[i], NULL);
	}
	writeToFile(argv[2]);

	return 0;
}
