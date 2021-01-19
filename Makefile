OBJ=$(patsubst src%, build%, $(patsubst %.cpp, %.o, $(SRC)))
SRC=$(shell find src -name '*.cpp')
HEAD=$(shell find src -name '*.h')
BIN=TinyLISP

LD=g++
LDFLAGS=
CXX=g++
CXXFLAGS=-std=c++17 -Wall -pedantic -Wno-long-long -c

## Default compile objects
.PHONY: all
all: $(OBJ)

## Run options
.PHONY: run help library letrec
run: $(BIN)
	./$(BIN)
help: $(BIN)
	./$(BIN) --help
library: $(BIN)
	./$(BIN) -f lib/lib.tl
letrec: $(BIN)
	./$(BIN) -f lib/letrec.tl

## Compile
$(BIN): $(OBJ)
	$(LD) $^ -o $@ $(LDFLAGS)

## META
.PHONY: doc clear clean echo

# Documentation
doc: 
	doxygen

# Clean
clear: clean
clean:
	rm -frd $(OBJ) $(BIN) Makefile.d doc

# Objects rule
$(OBJ): build/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

# Makefile dependencies
Makefile.d: $(SRC) $(HEAD)
	$(CXX) -MM $(SRC) | sed -E 's/(^.*\.o:)/build\/\1/' > Makefile.d
-include Makefile.d