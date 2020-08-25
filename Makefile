kelpchip: main.cpp chip8.cpp chip8.h
	clang++ -c main.cpp chip8.cpp
	clang++ main.o chip8.o -o kelpchip -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

clean:
	rm kelpchip
	rm *.o