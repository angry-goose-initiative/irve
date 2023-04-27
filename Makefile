# Makefile
# By: John Jekel

SOURCES = Makefile src/main.cpp

MAKEFLAGS += -j

all: irve.bin

irve: main.o 
	g++ -O3 -Wall -Wextra main.o

main.o: src/main.cpp
	g++ -O3 -Wall -Wextra -c src/main.cpp

clean:
	rm -f irve.bin *.o
