#all: 
#	g++ -Isrc/include -c main.cpp
#	g++ main.o -o main -Lsrc/lib -lsfml-graphics -lsfml-window -lsfml-system

output: main.o
	g++ main.o -o main -Lsrc/lib -lsfml-graphics -lsfml-window -lsfml-system

main.o: main.cpp
	g++ -Isrc/include -c main.cpp

	
