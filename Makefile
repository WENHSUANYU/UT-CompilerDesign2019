CXX=gcc
CXXFLAGS=-g -flto -Os -Wall
SRC=$(wildcard src/*.c)
BIN=scanner

all:
	$(CXX) -o $(BIN) $(SRC) $(CXXFLAGS)

clean:
	rm $(BIN)

run:
	./$(BIN)
