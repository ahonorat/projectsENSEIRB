DEBUG=YES

CC=gcc 
CFLAGS=-W -Wall -lpthread -fPIC #-DUSE_PTHREAD


ifeq ($(DEBUG),YES)
     CFLAGS += -g
else
     CFLAGS += -DNDEBUG
endif

## Répertoires tels que vus par la racine (se_emk/)
OBJ_DIR:=obj
BIN_DIR:=bin
LIB_DIR:=lib