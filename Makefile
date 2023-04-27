# Makefile
# By: John Jekel

SOURCES = Makefile src/*

MAKEFLAGS += -j

all: irve.bin

irve.bin: $(SOURCES)
	g++ -O3 -Wall -Wextra -o irve.bin src/*.cpp

clean:
	rm -f irve.bin
