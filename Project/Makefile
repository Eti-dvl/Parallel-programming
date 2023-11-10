# Add your names here!
NAMES ?= HAMELIN_BERNE

BIN := main
SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c,build/%.o,$(SRC))

CFLAGS := -Wall		# afficher tous les warnings
CFLAGS += -Iinc/ 	# headers .h dans inc/
CFLAGS += -fopenmp

DEBUG?=0
ifeq ($(DEBUG), 1)
# in debug configuration, disable optimizations
	CFLAGS+=-DDEBUG -g3 -gdwarf -O0
else
# in release mode, enable speed optimizations
	CFLAGS+=-DRELEASE -O3
endif

LDFLAGS := -lm -pthread -fopenmp 

CC := gcc
LD := gcc

$(BIN): $(OBJ)
	$(LD) -o $@ $^ $(LDFLAGS)

build/%.o: src/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	# clean compilation outputs
	rm -f $(OBJ) $(BIN) $(LOG)
	# clean the output of previous executions
	rm -f ./*.pgm 
	rm -f ./*.ppm

submit:
	echo "This will submit all your work in src/ inc/ and report/ directories"
	echo "Clean up, and prepare $(NAMES).tar.gz for submission."
	make clean
	tar -czvf $(NAMES).tar.gz src inc report

.PHONY: clean submit
