CC=gcc
CFLAGS=-W -Wall -std=c11
OMPFLAGS=-fopenmp
PTHREADFLAGS=-lpthread
SEQ=life_seq
OMP=life_omp
PTHREAD=life_pthread

compile_all: compile_seq compile_omp compile_pthread

compile_seq:
	$(CC) $(CFLAGS) $(SEQ).c -o $(SEQ)

compile_omp:
	$(CC) $(OMPFLAGS) $(CFLAGS) $(OMP).c -o $(OMP)

compile_pthread:
	$(CC) $(CFLAGS)  $(PTHREAD).c -o $(PTHREAD) $(PTHREADFLAGS)

clean: clean_seq clean_omp clean_pthread

clean_seq:
	rm $(SEQ)

clean_omp:
	rm $(OMP)

clean_pthread:
	rm $(PTHREAD)
