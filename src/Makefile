TEXT := >>>>>main compiled<<<<<

include conf.make


CFLAGS += -Iincludes
SRC:=example.c
OBJ:=example.o

OBJS=$(shell find ../obj -name '*.o')


#règles générales

all: exe ../bin/example

tests:
	@make -s -C prog_tests

#règles pour le main

exe:


../bin/example: ../obj/$(OBJ) $(OBJS)
	@$(CC) $(CFLAGS) $^ -o $@
ifeq ($(DEBUG),YES)
	$(info !!Sources compiled for debug!!)
endif
	$(info $(TEXT))


../obj/$(OBJ): $(SRC)
	@$(CC) -MMD $(CFLAGS) -c $< -o $@
	@cp ../obj/$*.d ./$(subst .c,.P,$(SRC)); \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
			-e '/^$$/ d' -e 's/$$/ :/' < ../obj/$*.d >> ./$(subst .c,.P,$(SRC)); \
		rm -f ../obj/$*.d

-include $(subst .c,.P,$(SRC))

#règles de mrpropre

clean: 	
	rm -f *.P
	rm -f ../bin/example ../obj/$(OBJ) $(subst .c,.P,$(SRC))
	@make -s -C core clean
	@make -s -C others clean
	@make -s -C prog_tests clean
