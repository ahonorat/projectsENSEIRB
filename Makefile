OBJ_DIR:=obj
SRC_DIR:=src
BIN_DIR:=bin


# main rules
all: dir main
	$(info =====>Compilation finished)

main:
	@make -s -C $(SRC_DIR)

tests:
	@make -s -C $(SRC_DIR) tests

# dirs
dir:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)

# clean
clean:  
	@make -s -C $(SRC_DIR) clean
	@rm -f $(OBJ_DIR)/* $(BIN_DIR)/*
