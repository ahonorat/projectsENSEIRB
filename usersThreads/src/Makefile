TEXT1 := >>>>>main compiled<<<<<
TEXT2 := >>>>>tests compiled<<<<
TEXT3 := >>>>>lib compiled<<<<<
TEXT4 := >>>>>tests compiled (using pthread)<<<<


include conf.make

CFLAGS += -Iincludes
SRC:=example.c
LIB:=$(shell find ../$(LIB_DIR) -name 'thread.so')
OBJS=$(shell find ../$(OBJ_DIR) -name '*.o')
HERE:=$(shell pwd)


#règles générales

ifeq ($(OPT),-DUSE_PTHREAD)
all: lib ../bin/example_pthread
else
all: lib ../bin/example
endif

tests: lib
	@make -s -C prog_tests
ifeq ($(DEBUG),YES)
	$(info !!Sources compiled for debug!!)
endif
ifeq ($(OPT),-DUSE_PTHREAD)
	$(info $(TEXT4))
else
	$(info $(TEXT2))
endif


ifeq ($(OPT),-DUSE_PTHREAD)
lib: 
else
lib: obj ../$(LIB_DIR)/thread.so
endif

obj:
	@make -s -C core
	@make -s -C others

../$(LIB_DIR)/thread.so: $(OBJS)
	@$(CC) $(CFLAGS) -o ../$(LIB_DIR)/thread.so -shared $(OBJS) $(LDFLAGS)
	$(info $(TEXT3))

#règles pour le main

ifeq ($(OPT),-DUSE_PTHREAD)
../bin/example_pthread: $(OBJS) $(SRC)
	@$(CC) $(CFLAGS) $(SRC) -o $@
ifeq ($(DEBUG),YES)
	$(info !!Sources compiled for debug!!)
endif
	$(info $(TEXT1))
else 
../bin/example: $(OBJS) $(SRC)
	@$(CC) $(CFLAGS) $(SRC) $(HERE)/../$(LIB_DIR)/thread.so -o $@ $(LDFLAGS)
ifeq ($(DEBUG),YES)
	$(info !!Sources compiled for debug!!)
endif
	$(info $(TEXT1))
endif


#règles de mrpropre

clean: 	
	rm -f *.P
	rm -f ../$(BIN_DIR)/example ../$(LIB_DIR)/thread.so $(subst .c,.P,$(SRC))
	@make -s -C core clean
	@make -s -C others clean
	@make -s -C prog_tests clean
