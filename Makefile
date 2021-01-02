OBJ=$(patsubst src%, build%, $(patsubst %.cpp, %.o, $(SRC)))
SRC=$(shell find src -name '*.cpp')
HEAD=$(shell find src -name '*.h')
BIN=TinyLISP

LD=g++
LDFLAGS=

CXX=g++
CXXDBGFLAGS= -g -D 'DBG' -D 'TEST'
CXXFLAGS=-std=c++17 -Wall -pedantic -Wno-long-long -c $(CXXDBGFLAGS)

all: $(OBJ)

compile: $(BIN)

run: compile
	./$(BIN)

.PHONY: all compile run doc clear clean echo

# Compile
$(BIN): $(OBJ)
	$(LD) $^ -o $@ $(LDFLAGS)

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