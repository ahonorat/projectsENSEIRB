CC=gcc
CFLAGS=-W -Wall -std=c11
OMPFLAGS=-fopenmp
PTHREADFLAGS=-lpthread
SEQ=life_seq
OMP=life_omp
PTHREAD=life_pthread
MPI_SRC=life_mpi.c util_mpi.c
MPI_BIN=life_mpi


all: $(SEQ) $(OMP) $(PTHREAD) $(MPI_BIN)

$(SEQ): $(SEQ).c
	$(CC) $(CFLAGS) $(SEQ).c -o $(SEQ)

$(OMP): $(OMP).c
	$(CC) $(OMPFLAGS) $(CFLAGS) $(OMP).c -o $(OMP)

$(PTHREAD): $(PTHREAD).c
	$(CC) $(CFLAGS)  $(PTHREAD).c -o $(PTHREAD) $(PTHREADFLAGS)

$(MPI_BIN): $(MPI_SRC)
	mpicc $(CFLAGS) $(MPI_SRC) -o $(MPI_BIN)

clean:
	rm $(SEQ)
	rm $(OMP)
	rm $(PTHREAD)
	rm $(MPI_BIN)
