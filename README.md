# JellyPhx
soft body physics simulation based on point-masses and springs

# How To Use
setup.cfg commands:                                                                                                
 w [resolution x] [resolution y]        # sets window size;                                                           
 g [gravity x] [gravity y]              # sets gravity;                                                              
 l [filename] [position x] [position y] # loads physics object from file;                                                            

# Compilation
run make
or
g++ -Isrc/include -c main.cpp
g++ main.o -o main -Lsrc/lib -lsfml-graphics -lsfml-window -lsfml-system

https://github.com/kiwioGIT/jellyPhyx/blob/phx-windows/showcase/image.png
