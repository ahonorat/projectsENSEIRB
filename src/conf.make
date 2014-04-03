DEBUG=YES

CC=gcc 
CFLAGS=-W -Wall -lpthread


ifeq ($(DEBUG),YES)
     CFLAGS += -g
endif
