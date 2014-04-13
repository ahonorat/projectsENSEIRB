DEBUG=YES

CC=gcc 
CFLAGS=-W -Wall -fPIC
CFLAGS+=$(OPT)

ifeq ($(DEBUG),YES)
     CFLAGS += -g
else
     CFLAGS += -DNDEBUG
endif

ifeq ($(OPT),-DUSE_PTHREAD)
     CFLAGS+= -pthread
endif


## Répertoires tels que vus par la racine (se_emk/)
OBJ_DIR:=obj
BIN_DIR:=bin
LIB_DIR:=lib