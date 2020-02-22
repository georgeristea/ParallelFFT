// RISTEA GEORGE VALENTIN 335CB
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <complex.h>
 
double PI;
typedef double complex cplx;
int N;
int P;
cplx* buf;
cplx* out;

void _fft(cplx buf[], cplx out[], int n, int step)
{
	if (step < n) {
		_fft(out, buf, n, step * 2);
		_fft(out + step, buf + step, n, step * 2);
 
		for (int i = 0; i < n; i += 2 * step) {
			cplx t = cexp(-I * PI * i / n) * out[i + step];
			buf[i / 2]     = out[i] + t;
			buf[(i + n)/2] = out[i] - t;
		}
	}
}
void* threadFunction(void *args)
{
	int thread_id = *(int*)args;
	if (P == 1) {
		_fft(buf, out, N, 1);
	}
	else if (P == 2) {
		if (thread_id == 0) {
			// apelez prima ramura
			_fft(out, buf, N, 2);
		}
		else {
			_fft(out + 1, buf + 1, N, 2);
			// apelez a doua ramura
		}
	}
	else if (P == 4) {
		if (thread_id == 0) {
			_fft(buf, out, N, 4);
		}
		else if (thread_id == 1) {
			_fft(buf + 2, out + 2, N, 4);
		}
		else if (thread_id == 2) {
			_fft(buf + 1, out + 1, N, 4);
		}
		else {
			_fft(buf + 3, out + 3, N, 4);
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

	buf = (cplx*) malloc(N * sizeof(cplx));
	out = (cplx*) malloc(N * sizeof(cplx));
	if (buf == NULL || out == NULL) {
		printf("[ERROR] malloc\n");
		exit(1);
	}
	for (i = 0; i < N; i++) {
		if (fscanf(fp, "%lf", &buf[i]) != 1) {
			printf("problem");
		}
	}
	for (i = 0; i < N; i++) {
		out[i] = buf[i];
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
		fprintf(fp, "%lf %lf\n", creal(buf[i]), cimag(buf[i]));
	}

	fclose(fp);

}
 
int main(int argc, char * argv[])
{
	readFromFile(argv[1]);
	P = atoi(argv[3]);
	PI = atan2(1, 1) * 4;


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
	// --------------------

	if (P == 2) {
		int step = 1;
		// trebuie sa construiesc solutia finala dupa ce
		// thread-urile au terminat
		for (i = 0; i < N; i += 2 * step) {
			cplx t = cexp(-I * PI * i / N) * out[i + step];
			buf[i / 2]     = out[i] + t;
			buf[(i + N)/2] = out[i] - t;
		}
	}
	else if (P == 4) {

		int step  = 2;
		// trebuie sa construiesc solutia pentru ultimele 2 threaduri
		for (int i = 0; i < N; i += 2 * step) {
			cplx t = cexp(-I * PI * i / N) * buf[i + step];
			out[i / 2]     = buf[i] + t;
			out[(i + N)/2] = buf[i] - t;
		}
		// trebuie sa construiesc solutia pentru primele 2 thread-uri
		for (int i = 0; i < N; i += 2 * step) {
			
			cplx t = cexp(-I * PI * i / N) * buf[(i + 1) + step];
			out[(i / 2) + 1]     = buf[i + 1] + t;
			out[(i + N) / 2 + 1] = buf[i + 1] - t;
		}
		step /= 2;
		// trebuie sa construiesc solutia finala
		for (int i = 0; i < N; i += 2 * step) {
			cplx t = cexp(-I * PI * i / N) * out[i + step];
			buf[i / 2]     = out[i] + t;
			buf[(i + N)/2] = out[i] - t;
		}
	}

	writeToFile(argv[2]);
 
	return 0;
}
 