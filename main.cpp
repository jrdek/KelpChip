#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include "chip8.h"


void update_screen(unsigned char* gfx_map, sf::Texture* screen_txt, sf::Sprite* screen) {
	screen_txt->update(gfx_map);
	screen->setTexture(*screen_txt);
}


int main(int argc, char** argv) {

	chip8 cpu = chip8();
	cpu.load_program(argv[1]);

	sf::RenderWindow window(sf::VideoMode(640,320), "chip8 emu");

	sf::Sprite screen;
	screen.setPosition(0, 0);
	screen.setScale(10, 10);
	sf::Texture screen_txt;
	screen_txt.create(64, 32);

	sf::SoundBuffer beep_buff;
	beep_buff.loadFromFile("beep.wav");
	sf::Sound beep;
	beep.setBuffer(beep_buff);

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			} else if (event.type == sf::Event::KeyPressed) {
				switch (event.key.code) {
					/*
					Keypad layout is as follows:

					0 1 2 3
					4 5 6 7
					8 9 A B
					C D E F

					pressed by 

					4 5 6 7
					R T Y U
					F G H J
					V B N M
					*/
					case sf::Keyboard::Num4:
						*cpu.keys |= 1;
						break;
					case sf::Keyboard::Num5:
						*cpu.keys |= 2;
						break;
					case sf::Keyboard::Num6:
						*cpu.keys |= 4;
						break;
					case sf::Keyboard::Num7:
						*cpu.keys |= 8;
						break;
					case sf::Keyboard::R:
						*cpu.keys |= 16;
						break;
					case sf::Keyboard::T:
						*cpu.keys |= 32;
						break;
					case sf::Keyboard::Y:
						*cpu.keys |= 64;
						break;
					case sf::Keyboard::U:
						*cpu.keys |= 128;
						break;
					case sf::Keyboard::F:
						*cpu.keys |= 256;
						break;
					case sf::Keyboard::G:
						*cpu.keys |= 512;
						break;
					case sf::Keyboard::H:
						*cpu.keys |= 1024;
						break;
					case sf::Keyboard::J:
						*cpu.keys |= 2048;
						break;
					case sf::Keyboard::V:
						*cpu.keys |= 4096;
						break;
					case sf::Keyboard::B:
						*cpu.keys |= 8192;
						break;
					case sf::Keyboard::N:
						*cpu.keys |= 16384;
						break;
					case sf::Keyboard::M:
						*cpu.keys |= 32768;
						break;
				}
			} else if (event.type == sf::Event::KeyReleased) {
				switch (event.key.code) {
					case sf::Keyboard::Num4:
						*cpu.keys &= !1u;
						break;
					case sf::Keyboard::Num5:
						*cpu.keys &= !2u;
						break;
					case sf::Keyboard::Num6:
						*cpu.keys &= !4u;
						break;
					case sf::Keyboard::Num7:
						*cpu.keys &= !8u;
						break;
					case sf::Keyboard::R:
						*cpu.keys &= !16u;
						break;
					case sf::Keyboard::T:
						*cpu.keys &= !32u;
						break;
					case sf::Keyboard::Y:
						*cpu.keys &= !64u;
						break;
					case sf::Keyboard::U:
						*cpu.keys &= !128u;
						break;
					case sf::Keyboard::F:
						*cpu.keys &= !256u;
						break;
					case sf::Keyboard::G:
						*cpu.keys &= !512u;
						break;
					case sf::Keyboard::H:
						*cpu.keys &= !1024u;
						break;
					case sf::Keyboard::J:
						*cpu.keys &= !2048u;
						break;
					case sf::Keyboard::V:
						*cpu.keys &= !4096u;
						break;
					case sf::Keyboard::B:
						*cpu.keys &= !8192u;
						break;
					case sf::Keyboard::N:
						*cpu.keys &= !16384u;
						break;
					case sf::Keyboard::M:
						*cpu.keys &= !32768u;
						break;
				}
			}
		}

		// run the next step
		cpu.exec_next();

		window.clear();
		update_screen(cpu.gfx_colors, &screen_txt, &screen);
		window.draw(screen);
		window.display();

		if (*cpu.sound_timer) beep.play();
	}
	return 0;
}