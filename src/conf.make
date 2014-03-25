DEBUG=YES

CC=gcc 
CFLAGS=-W -Wall


ifeq ($(DEBUG),YES)
     CFLAGS += -g
endif
