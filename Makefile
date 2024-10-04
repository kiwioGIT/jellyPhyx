all: 
	g++ -Isrc/include -c main/main.cpp
	g++ main.o -o main -Lsrc/lib -lsfml-graphics -lsfml-window -lsfml-system


