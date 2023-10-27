
# ----------------------------------------------------------------------------
# Variables
# ----------------------------------------------------------------------------

# $(wildcard *.c) equivalent to `ls *.c` in Linux shell
SRC := $(wildcard *.c)
# Take all names in SRC, replace ending .c with .o
OBJ := $(patsubst %.c,%.o,$(SRC))
# The target program
BIN := prod-cons

# Compilation flags
CFLAGS := -O0 -g 	# no optimization ; use debugging symbols (for GDB)
CFLAGS += -I./ 	# headers should be in ./ directory
CFLAGS += -Wall		# display all warning

# Link flags: add here all required libraries
LDFLAGS := -lm -lpthread	

# Compiler & linker command name
CC := gcc
LD := gcc

# ----------------------------------------------------------------------------
# Compilation rules
# ----------------------------------------------------------------------------

# Generic structure of a compilation rule: 
# target: prerequisite1, prerequisite2, ...	target and prerequisites are usually file names
# 	command1 (note that line starts with a TAB, so be careful with spaces!)
#	command2

# Some useful automatic variables in make:
# % : pattern matching
# $@ : the target of current rule
# $^ : all listed prerequisites
# $< : the first prerequisite listed

# Main program compilation rule: to build prod-cons, you need all .o in build/
$(BIN): $(OBJ)
	$(LD) -o $@ $^ $(LDFLAGS)

# to build build/some_obj.o you need src/some_obj.c
# and you compile it with: gcc -O0 -g -Iinc/ -Wall -o build/tutorial.o -c src/tutorial.c
%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

# clean up rule
clean:
	rm -f $(OBJ) $(BIN)

# measure the execution time with 2 producers and 10 consumers
time: $(BIN)
	/usr/bin/time -p ./$(BIN) 2 10

# the 'clean' target is not a file
.PHONY: clean time
