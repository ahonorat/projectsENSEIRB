TEXT := >>>>>Compilation of a core file finished<<<<<

include ../conf.make


CFLAGS += -I../includes
SRCS=$(wildcard *.c) 
OBJS=$(patsubst %.c,../../$(OBJ_DIR)/%.o,$(SRCS))


all:$(OBJS)


../../obj/%.o:%.c
	@$(CC) -MMD $(CFLAGS) -c $< -o $@ $(LDFLAGS)
	@cp ../../$(OBJ_DIR)/$*.d ./$*.P; \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
			-e '/^$$/ d' -e 's/$$/ :/' < ../../$(OBJ_DIR)/$*.d >> ./$*.P; \
		rm -f ../../$(OBJ_DIR)/$*.d
	$(info $(TEXT))
	$(info $*.c)

-include *.P

clean: 
	rm -f *.P
	rm -f $(OBJS)

