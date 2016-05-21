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
// #include <sched.h>

#define sec(t) (t.tv_sec + (t.tv_usec/1000000.))

#define N_TYPES 1

/*
	arg é tratado como um size_t N
	A função executa um loop N^2 vezes e executa um exit(0)
*/
void* looping(void *arg) {
	size_t n = (size_t) arg;

	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++);

	return NULL;
}

/*
	int getrusage(int who, struct rusage *usage);
	Reference: http://linux.die.net/man/2/getrusage
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
	int n_threads_type[N_TYPES];
	void* (*fns[N_TYPES])(void*) = { looping, NULL, NULL, NULL };

	for (int i = 0; i < N_TYPES; ++i) {
		n_threads_type[i] = atoi(argv[i+4]);
		n_threads += n_threads_type[i];
	}

	pthread_t threads[n_threads];

	int pos = 0;
	for (int i = 0; i < N_TYPES; ++i) {
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

		// ID, etime, utime, stime, pcpu, pcpu_per_thread
		printf("%s %f %f %f %f %f\n", ID, etime, utime, stime, pcpu, pcpu/n_threads);
		fflush(stdout);
	}

	return 0;
}
