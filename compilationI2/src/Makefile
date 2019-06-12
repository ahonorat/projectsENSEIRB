LEX=lex
YACC=yacc -v
CFLAGS=-Wall -g
CC=gcc

all:bin/parse drive

bin/parse:y.tab.c lex.yy.c tree.c concat.c
	mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $^

y.tab.c:grammar.y
	$(YACC) -d $^ 

lex.yy.c:scanner.l
	$(LEX) -o $@ $^

clean:
	rm -f result/drive.ll result/result.ll result/test.ll result/result_test.ll lex.yy.c y.tab.c y.tab.h
	rm -rf bin/

drive:
	mkdir -p result
	./bin/parse input/drive.input > result/result.ll
	cat result/entete.ll result/result.ll result/pied.ll > result/drive.ll

test:
	mkdir -p result
	./bin/parse input/test.c > result/result_test.ll
	cat result/entete.ll result/result_test.ll result/pied.ll > result/test.ll
