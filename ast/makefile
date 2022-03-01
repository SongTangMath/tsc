CC = g++
CFLAGS = -std=c++11 -g -Wall -Wno-deprecated-register

a.out: parser.cpp lex.yy.cpp tsc.tab.cpp
	$(CC) $(CFLAGS)  -o a.out parser.cpp lex.yy.cpp tsc.tab.cpp
	
lex.yy.cpp: tsc.l parser.h
	flex --header-file=lex.yy.h -o lex.yy.cpp tsc.l
tsc.tab.cpp: tsc.y parser.h	
	bison -d tsc.y -o tsc.tab.cpp
clean:
	rm -f a.out lex.yy.cpp lex.yy.h tsc.tab.cpp tsc.tab.hpp
