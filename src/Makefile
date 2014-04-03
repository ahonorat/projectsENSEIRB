TEXT1 := >>>>>main compiled<<<<<
TEXT2 := >>>>>tests compiled<<<<


include conf.make


CFLAGS += -Iincludes
SRC:=example.c
OBJ:=example.o

OBJS=$(shell find ../obj -name '*.o')


#règles générales

all: exe ../bin/example

tests: exe
	@make -s -C prog_tests
ifeq ($(DEBUG),YES)
	$(info !!Sources compiled for debug!!)
endif
	$(info $(TEXT2))

exe:
	@make -s -C core
	@make -s -C others

#règles pour le main


../bin/example: $(OBJS) ../obj/$(OBJ)
	@$(CC) $(CFLAGS) $(OBJS) -o $@
ifeq ($(DEBUG),YES)
	$(info !!Sources compiled for debug!!)
endif
	$(info $(TEXT1))


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
