#include <stdio.h>
#include <stdlib.h>

#include "util/perf.h"
#include "benchmark/bench.h"

#define N_ITER 10
#define BENCH_I_MIN 64

void bench_generic( char* filename, int i_max,
                    long (*flop)(long i),
                    void (*process)(void* data, int i),
                    void* data)
{
    FILE* logfile = fopen(filename, "w+");

    printf("Benchmark > %s\n",filename);
    printf("-------\n");
    printf("Iter\tMFlop/s\n");
    long i;
    //On incrémente i de 25%
    for(i=BENCH_I_MIN; i<=i_max; i+=i/4)
    {
        perf_t start,stop;
        perf(&start);

        int j;
	// le calcul est lancé plusieurs fois afin d'obtenir
	// des performances non faussées par d'autres processus
        for(j=0;j<N_ITER;j++)
            process(data, i);

        perf(&stop);
        perf_diff(&start, &stop);
        double mflops = perf_mflops(&stop, flop(i)*N_ITER);

	//Ecriture du resultat dans le logfile et dans la console
        fprintf(logfile,"%lu\t%f\n",i,mflops);
        printf("%lu\t%f\n",i,mflops);
    }
    fclose(logfile);
}
