TEXT := >>>>>Compilation of a tests file finished<<<<<

include ../conf.make


CFLAGS += -I../includes
SRCS=$(wildcard *.c) 
OBJS=$(shell find ../../obj -name '*.o')
EXES=$(patsubst %.c,../../bin/%,$(SRCS))


all:$(EXES)


../../bin/%:%.c
	@$(CC) $(CFLAGS) $< $(patsubst ../../obj/example.o,,$(OBJS)) -o $@
	$(info $(TEXT))
	$(info $*.c)


clean: 
	rm -f $(EXES)

