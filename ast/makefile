CC = g++
CFLAGS = -std=c++11 -g -Wall -Wno-deprecated-register


a.out: lex.yy.o tsc.tab.o  parser.o semantics_analysis.o main.o 
	$(CC) parser.o main.o tsc.tab.o semantics_analysis.o lex.yy.o

lex.yy.o: lex.yy.cpp tsc.tab.cpp
	$(CC) $(CFLAGS) -c -o lex.yy.o lex.yy.cpp
tsc.tab.o: lex.yy.cpp tsc.tab.cpp
	$(CC) $(CFLAGS) -c -o tsc.tab.o tsc.tab.cpp

parser.o: parser.cpp 
	$(CC) $(CFLAGS) -c -o parser.o parser.cpp 
semantics_analysis.o: semantics_analysis.cpp 
	$(CC) $(CFLAGS) -c -o semantics_analysis.o semantics_analysis.cpp 

main.o:
	$(CC) $(CFLAGS) -c -o main.o main.cpp

lex.yy.cpp: tsc.l parser.h
	flex --header-file=lex.yy.h -o lex.yy.cpp tsc.l
tsc.tab.cpp: tsc.y parser.h	
	bison -d tsc.y -o tsc.tab.cpp


clean:
	rm -f *.out *.o lex.yy.cpp lex.yy.h tsc.tab.cpp tsc.tab.hpp
