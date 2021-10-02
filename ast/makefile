CC = gcc
CFLAGS = -std=c++11

all: test.out

test.out: test.o token.o
	g++ -o test.out test.o token.o

test.o: test.cpp
	$(CC) $(CFLAGS) test.cpp -o test.o -c

token.o: token.cpp
	$(CC) $(CFLAGS) token.cpp -o token.o -c