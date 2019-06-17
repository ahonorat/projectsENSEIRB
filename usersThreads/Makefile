include src/conf.make

# main rules
all: lib main

main: dir
	@make -s -C src

lib: dir
	@make -s -C src lib

tests: dir lib
	@make -s -C src tests

# dirs
dir:
	@mkdir -p $(LIB_DIR)
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)

# clean
clean:  
	@make -s -C src clean
	@rm -f $(OBJ_DIR)/* $(BIN_DIR)/* $(LIB_DIR)/*
