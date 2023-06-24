# The IRVE Makefile
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.

CPP_SOURCES = src/main.cpp lib/*.cpp

MAKEFLAGS += -j

all: irve.bin

.PHONY: irve.bin
irve.bin:
	g++ -DIRVE_RAW_MAKEFILE_BUILDSYSTEM -fsanitize=address -std=c++20 -O0 -g -Wall -Wextra -o irve.bin -I src -I lib -I include ${CPP_SOURCES}

clean:
	rm -f irve.bin
