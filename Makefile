CXX=gcc
CXXFLAGS=-g -flto -Os -Wall
SRC=$(wildcard src/*.c)
BIN=scanner
.PHONY: test

all:
	$(CXX) -o $(BIN) $(SRC) $(CXXFLAGS)

clean:
	rm $(BIN)

run:
	./$(BIN)

test:
	./test/scanner_test.sh
