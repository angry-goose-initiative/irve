# The IRVE Makefile
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.

SOURCES = Makefile src/*

MAKEFLAGS += -j

all: irve.bin

irve.bin: $(SOURCES)
	g++ -fsanitize=address -std=c++20 -O0 -g -Wall -Wextra -o irve.bin src/*.cpp

clean:
	rm -f irve.bin
