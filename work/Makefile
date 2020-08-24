test: main.cpp chip8.cpp chip8.h
	clang++ -c main.cpp chip8.cpp
	clang++ main.o chip8.o -o test -lsfml-graphics -lsfml-window -lsfml-system

clean:
	rm test
	rm *.o