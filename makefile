CC = g++
CFLAGS = -std=c++11 -g -Wall

all: 
	make -C ./ast

clean: 
	make clean -C ./ast