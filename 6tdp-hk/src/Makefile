CC=gcc
CFLAGS=-W -Wall -std=c11
OMPFLAGS=-fopenmp
PTHREADFLAGS=-lpthread
SEQ=life_seq
OMP=life_omp
PTHREAD=life_pthread
MPI_SRC=life_mpi.c util_mpi.c
MPI_BIN=life_mpi


compile_all: compile_seq compile_omp compile_pthread compile_mpi

compile_seq:
	$(CC) $(CFLAGS) $(SEQ).c -o $(SEQ)

compile_omp:
	$(CC) $(OMPFLAGS) $(CFLAGS) $(OMP).c -o $(OMP)

compile_pthread:
	$(CC) $(CFLAGS)  $(PTHREAD).c -o $(PTHREAD) $(PTHREADFLAGS)

compile_mpi:
	mpicc $(CFLAGS) $(MPI_SRC) -o $(MPI_BIN)

clean: clean_seq clean_omp clean_pthread clean_mpi

clean_seq:
	rm $(SEQ)

clean_omp:
	rm $(OMP)

clean_pthread:
	rm $(PTHREAD)

clean_mpi:
	rm $(MPI_BIN)
