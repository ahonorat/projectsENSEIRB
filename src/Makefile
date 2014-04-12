TEXT1 := >>>>>main compiled<<<<<
TEXT2 := >>>>>tests compiled<<<<
TEXT3 := >>>>>lib compiled<<<<<<


include conf.make

CFLAGS += -Iincludes
SRC:=example.c
LIB:=$(shell find ../$(LIB_DIR) -name '*.so')
OBJS=$(shell find ../$(OBJ_DIR) -name '*.o')
HERE:=$(shell pwd)


#règles générales

all: lib ../bin/example

tests: lib
	@make -s -C prog_tests
ifeq ($(DEBUG),YES)
	$(info !!Sources compiled for debug!!)
endif
	$(info $(TEXT2))

lib: obj ../$(LIB_DIR)/thread.so

obj:
	@make -s -C core
	@make -s -C others

../$(LIB_DIR)/thread.so: $(OBJS)
	@$(CC) $(CFLAGS) -o ../$(LIB_DIR)/thread.so -shared $(OBJS)
	$(info $(TEXT3))

#règles pour le main


../bin/example: $(OBJS) $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(HERE)/../$(LIB_DIR)/thread.so -o $@
ifeq ($(DEBUG),YES)
	$(info !!Sources compiled for debug!!)
endif
	$(info $(TEXT1))
	$(info $(HERE)/../$(LIB_DIR))

#règles de mrpropre

clean: 	
	rm -f *.P
	rm -f ../$(BIN_DIR)/example ../$(LIB_DIR)/thread.so $(subst .c,.P,$(SRC))
	@make -s -C core clean
	@make -s -C others clean
	@make -s -C prog_tests clean
