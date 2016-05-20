/* 
** Universidade Federal de S�o Carlos
** Departamento de Computa��o
** Prof. H�lio Crestana Guardia
** Sistemas Operacionais 2
*/

/*
** Programa :
** Comando: getrusage()
** Objetivo: observar consumo de recursos pelo processo com getrusage
*/

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>

#define sec(t) (t.tv_sec + (t.tv_usec/1000000.))

/*
	int getrusage(int who, struct rusage *usage);

	getrusage returns the  current  resource  usages,  for  a  who  of  either
	RUSAGE_SELF or RUSAGE_CHILDREN.  The former asks for resources used by the
	current process, the latter for resources used by those  of  its  children
	that have terminated and have been waited for.

	struct rusage {
		struct timeval ru_utime; // user time used
		struct timeval ru_stime; // system time used
		long   ru_maxrss;        // maximum resident set size
		long   ru_ixrss;         // integral shared memory size
		long   ru_idrss;         // integral unshared data size
		long   ru_isrss;         // integral unshared stack size
		long   ru_minflt;        // page reclaims
		long   ru_majflt;        // page faults
		long   ru_nswap;         // swaps
		long   ru_inblock;       // block input operations
		long   ru_oublock;       // block output operations
		long   ru_msgsnd;        // messages sent
		long   ru_msgrcv;        // messages received
		long   ru_nsignals;      // signals received
		long   ru_nvcsw;         // voluntary context switches
		long   ru_nivcsw;        // involuntary context switches
	};

	struct timeval {
		time_t      tv_sec;	// seconds
		suseconds_t tv_usec;	// microseconds
	};
*/

int
main()
{
	struct timeval inic, fim;
	struct rusage ru_i, ru_f;

	float etime, utime, stime, pcpu;

	int i,j;

	// ...


	// Verifica o instante atual antes do bloco de c�digo sendo medido
	gettimeofday(&inic,0);

	// Verifica o consumo de recursos at� o bloco de c�digo sendo medido
	// int getrusage(int who, struct rusage *usage);
	// RUSAGE_SELF or RUSAGE_CHILDREN
	getrusage(RUSAGE_SELF,&ru_i);

	// Aqui vai o c�digo cujo consumo se deseja medir
	// ...

	for(i=0;i<100000;i++)
		for(j=0;j<10000;j++);

	// Verifica o instante atual logo depois do bloco de c�digo sendo medido
	gettimeofday(&fim,0);

	// Verifica o consumo de recursos logo depois do bloco de c�digo sendo medido
	getrusage(RUSAGE_SELF,&ru_f);

	// tempo decorrido: elapsed time
	etime = (fim.tv_sec + fim.tv_usec/1000000.) -
			(inic.tv_sec + inic.tv_usec/1000000.) ;

	// tempo na CPU: user time
	utime = (ru_f.ru_utime.tv_sec + ru_f.ru_utime.tv_usec/1000000.) -
			(ru_i.ru_utime.tv_sec + ru_i.ru_utime.tv_usec/1000000.) ;

	// tempo de servi�os do SO: system time
	stime = (ru_f.ru_stime.tv_sec + ru_f.ru_stime.tv_usec/1000000.) -
			(ru_i.ru_stime.tv_sec + ru_i.ru_stime.tv_usec/1000000.) ;

	// porcentagem de uso da CPU: (utime + stime) / etime;
	pcpu = (utime + stime) / etime;

	printf("\n");
	printf("Tempo decorrido: %f s\n", etime);
	printf("User time: %f s\n", utime);
	printf("System time: %f s\n", stime);
	printf("Porcentagem de uso da cpu: %f \% \n\n", pcpu);

	return(0);
}
