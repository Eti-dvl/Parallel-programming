
# ----------------------------------------------------------------------------
# Variables
# ----------------------------------------------------------------------------

# $(wildcard *.c) equivalent to `ls *.c` in Linux shell
SRC := $(wildcard *.c)
# Take all names in SRC, replace ending .c with .o
OBJ := $(patsubst %.c,%.o,$(SRC))
# The target program
BIN := ln2

# The timing measurement file
CSV := $(BIN).csv

# Compilation flags
CFLAGS := -O0 -g 	# no optimization ; use debugging symbols (for GDB)
CFLAGS += -I./ 		# headers should be in ./ directory
CFLAGS += -fopenmp	# use openMP compilation directives
CFLAGS += -Wall		# display all warning

# Link flags: add here all required libraries
LDFLAGS := -lm -lpthread -fopenmp

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

# measure execution times
$(CSV): $(BIN)
	@rm -f $(CSV)
	@echo "k;real" > $(CSV)
	$(foreach k, 1 2 3 4 5 6 7 8 9 10, time -f "$(BIN);$(k);%e" -o $(CSV) -a ./$(BIN) $(k);)
	sed -i -e "s/\./,/g" -e "s/;/\t/g" $(CSV)

# clean up rule
clean:
	rm -f $(OBJ) $(BIN) $(CSV)

# the 'clean' target is not a file
.PHONY: clean

