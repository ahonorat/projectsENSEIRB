TEXT := >>>>>Compilation of a test file finished<<<<<

include ../conf.make


CFLAGS += -I../includes
SRCS=$(wildcard *.c) 
OBJS=$(shell find ../../$(OBJ_DIR) -name '*.o')
EXES=$(patsubst %.c,../../bin/%,$(SRCS))
LIB:=$(shell find ../../$(LIB_DIR) -name '*.so')
HERE:=$(shell pwd)


all:$(EXES)


../../bin/%:%.c
	@$(CC) $(CFLAGS) $< $(HERE)/../../$(LIB_DIR)/thread.so -o $@
	$(info $(TEXT))
	$(info $*.c)


clean: 
	rm -f $(EXES)

