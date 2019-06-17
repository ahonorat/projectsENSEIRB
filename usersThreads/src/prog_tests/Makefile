TEXT1 := >>>>>Compilation of a test file finished (using pthread)<<<<<
TEXT2 := >>>>>Compilation of a test file finished<<<<<

include ../conf.make


CFLAGS += -I../includes
SRCS=$(shell find -name '[0-9]*.c')
OBJS=$(shell find ../../$(OBJ_DIR) -name '*.o')
EXES=$(patsubst %.c,../../$(BIN_DIR)/%,$(SRCS))
LIB:=$(shell find ../../$(LIB_DIR) -name '*.so')
HERE:=$(shell pwd)

ifeq ($(OPT),-DUSE_PTHREAD)
all:$(patsubst %,%_pthread,$(EXES))
else
all:$(EXES)
endif

ifeq ($(OPT),-DUSE_PTHREAD)
../../$(BIN_DIR)/%_pthread:%.c
	@$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	$(info $(TEXT1))
	$(info $*.c)
else
../../$(BIN_DIR)/%:%.c
	@$(CC) $(CFLAGS) $< $(HERE)/../../$(LIB_DIR)/thread.so -o $@ $(LDFLAGS)
	$(info $(TEXT2))
	$(info $*.c)
endif

contextes:contextes.c
	@$(CC) $(CFLAGS) $< -o ../../$(BIN_DIR)/$@

clean: 
	rm -f $(EXES)

