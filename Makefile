# Makefile
# By: John Jekel

SOURCES = Makefile src/*

MAKEFLAGS += -j

all: irve.bin

irve.bin: $(SOURCES)
	g++ -fsanitize=address -std=c++20 -O0 -g -Wall -Wextra -o irve.bin src/*.cpp

clean:
	rm -f irve.bin
