include src/conf.make

TESTS=$(shell find $(BIN_DIR) -name '[0-9]*')

# main rules
all: dir lib main
	$(info =====>Compilation finished)

main:
	@make -s -C src

lib:
	@make -s -C src lib

tests: dir lib
	@make -s -C src tests

bench:
	$(shell tests.sh)

# dirs
dir:
	@mkdir -p $(LIB_DIR)
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)

# clean
clean:  
	@make -s -C src clean
	@rm -f $(OBJ_DIR)/* $(BIN_DIR)/* $(LIB_DIR)/*
