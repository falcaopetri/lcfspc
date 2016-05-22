/*
 *	 Universidade Federal de São Carlos
 * 		Departamento de Computação
 *
 *	   Prof. Hélio Crestana Guardia
 *		  Sistemas Operacionais 1
 * 				2016/1
 *
 * 	Author: Antonio Carlos Falcão Petri
 */

/*
 * Program:
 * Command: getrusage()
 * Goal: observar consumo de recursos pelo processo com getrusage
 */

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define sec(t) (t.tv_sec + (t.tv_usec/1000000.))

#define N_FN_TYPES 3

/*
	arg is treated as a size_t N
	this functions executes an empty loop N^2 times
*/
void* looping(void *arg) {
	size_t n = (size_t) arg;

	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++);

	return NULL;
}

/*
	Reference: http://www.cplusplus.com/reference/cstdlib/qsort/
*/
int comp_int(const void* p1, const void* p2) {
	int v1 = *((int*) p1);
	int v2 = *((int*) p2);

	if (v1 < v2) return -1;
	if (v1 == v2) return 0;

	return 1;
}

/*
	arg is treated as a size_t N
	Sorts an array of size N, with random values in range [0, 100000)
*/
void* sorting(void *arg) {
	size_t n = (size_t) arg;
	int *vet = (int*) malloc(n * sizeof(int));

	for (int i = 0; i < n; ++i) vet[i] = rand() % 100000;

	qsort(vet, n, sizeof(int), comp_int);

	free(vet);

	return NULL;
}

/*
	arg is treated as a size_t N
	Writes to a file called "tmp" + ID N random numbers
	Deletes file from disk after finished
	TODO force disk syncing, maybe with open() from fcntl.h
*/
void* writing_to_file(void *arg) {
	char filename[20];
	strcpy(filename, "tmp");
	size_t tid = (size_t) pthread_self();
	sprintf(filename, "tmp%lu", tid);

	FILE *file = fopen(filename, "w+");
	size_t n = (size_t) arg;

	for (int i = 0; i < n; ++i) fprintf(file, "%d\n", rand() % 100000);

	fclose(file);
	remove(filename);

	return NULL;
}

/*
	int getrusage(int who, struct rusage *usage);
	Reference: http://linux.die.net/man/2/getrusage

	TODO N_F4 not currently used
	Maybe some system-call based function, or a mixed one?
*/
int main(int argc, char *argv[]) {
	if (argc < 8) {
		printf("USAGE:\n");
		printf("./a.out $ID $PRINT_INTERVAL $DATA $N_F1 $N_F2 $N_F3 $N_F4\n");
		return 1;
	}

	struct timeval inic, fim;
	struct rusage ru_i, ru_f;

	float etime, utime, stime, pcpu;

	char *ID = argv[1];
	size_t PRINT_INTERVAL = atoi(argv[2]);
	size_t DATA = atoi(argv[3]);

	int n_threads = 0;
	int n_threads_type[N_FN_TYPES];
	void* (*fns[N_FN_TYPES])(void*) = { looping, sorting, writing_to_file, NULL };

	for (int i = 0; i < N_FN_TYPES; ++i) {
		n_threads_type[i] = atoi(argv[i+4]);
		n_threads += n_threads_type[i];
	}

	pthread_t threads[n_threads];

	// Creates the number of specified threads of each type
	int pos = 0;
	for (int i = 0; i < N_FN_TYPES; ++i) {
		for (int j = 0; j < n_threads_type[i]; ++j) {
			if (pthread_create(&threads[pos++], NULL, fns[i], (void*) DATA)) {
				return 1;
			}
		}
	}

	// TODO exit after all the other threads have finished
	while (1) {
		// Verifica o instante atual antes do bloco de código sendo medido
		gettimeofday(&inic, 0);
		// Verifica o consumo de recursos at� o bloco de código sendo medido
		getrusage(RUSAGE_SELF, &ru_i);

		sleep(PRINT_INTERVAL);

		// Verifica o instante atual logo depois do bloco de código sendo medido
		gettimeofday(&fim, 0);
		// Verifica o consumo de recursos logo depois do bloco de c�digo sendo medido
		getrusage(RUSAGE_SELF, &ru_f);

		// tempo decorrido: elapsed time
		etime = sec(fim) - sec(inic);
		// tempo na CPU: user time
		utime = sec(ru_f.ru_utime) - sec(ru_i.ru_utime);
		// tempo de serviços do SO: system time
		stime = sec(ru_f.ru_stime) - sec(ru_i.ru_stime);

		// porcentagem de uso da CPU: (utime + stime) / etime;
		pcpu = (utime + stime) / etime; // *100 ???

		// TODO pcpu/n_threads não é confiável: deve-se levar em consideração
		// quantas threads estão ativas
		// ID, etime, utime, stime, pcpu, pcpu_per_thread
		printf("%s %f %f %f %f %f\n", ID, etime, utime, stime, pcpu, pcpu/n_threads);
		fflush(stdout);
	}

	return 0;
}
