CC = g++
CFLAGS = -std=c++11 -g -Wall

all: test.out

test.out: test.o token.o node.o
	$(CC) $(CFLAGS) -o test.out test.o token.o node.o

test.o: test.cpp
	$(CC) $(CFLAGS) test.cpp -o test.o -c

token.o: ast/token.cpp
	$(CC) $(CFLAGS) ast/token.cpp -o token.o -c

node.o: ast/node.cpp
	$(CC) $(CFLAGS) ast/node.cpp -o node.o -c

clean:
	rm -f *.o *.out