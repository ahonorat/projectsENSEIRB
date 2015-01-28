CC=gcc
CFLAGS=-W -Wall -std=c11
OMPFLAGS=-fopenmp
SEQ=life_seq
OMP=life_omp

compile_all: compile_seq compile_omp

compile_omp:
	$(CC) $(OMPFLAGS) $(CFLAGS) $(OMP).c -o $(OMP)

compile_seq:
	$(CC) $(CFLAGS) $(SEQ).c -o $(SEQ)

clean: clean_seq clean_omp
	rm $(SEQ) $(OMP)

clean_seq:
	rm $(SEQ)

clean_omp:
	rm $(OMP)
