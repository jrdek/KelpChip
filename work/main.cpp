#include <SFML/Graphics.hpp>
#include <iostream>
#include "chip8.h"



void update_and_draw_pixels(unsigned char* gfx_map, sf::RectangleShape* pixels, sf::RenderWindow* window) {

	// for (int c = 0; c < (64*4); c++) {
	// 	for (int bit = 0; bit < 8; bit++) {
	// 		pixels[(7-bit)+(8*c)].setFillColor((gfx_map[c] & (1 << bit)) ? sf::Color::White : sf::Color::Black);
	// 		window->draw(pixels[(7-bit)+(8*c)]);
	// 	}
	// }  // god i hope this works

	for (int c = 0; c < 64*32; c++) {
		pixels[c].setFillColor(gfx_map[c] ? sf::Color::White : sf::Color::Black);
		window->draw(pixels[c]);
	}
}


int main(int argc, char** argv) {

	chip8 cpu = chip8();
	cpu.load_program(argv[1]);

	// // screen testing... uncomment the wacky formula for setPosition to use this.
	// for (int i = 0; i < 32; i++) {
	// 	cpu.gfx[8*i] = cpu.mem[5*(i%16)];
	// 	cpu.gfx[8*i+1] = cpu.mem[5*(i%16)+1];
	// 	cpu.gfx[8*i+2] = cpu.mem[5*(i%16)+2];
	// 	cpu.gfx[8*i+3] = cpu.mem[5*(i%16)+3];
	// 	cpu.gfx[8*i+4] = cpu.mem[5*(i%16)+4];
	// 	cpu.gfx[8*i+5] = 0x00;
	// 	cpu.gfx[8*i+6] = 0x00;
	// 	cpu.gfx[8*i+7] = 0x00;
	// }

	sf::RectangleShape pixels[64*32];

	sf::RenderWindow window(sf::VideoMode(640,320), "chip8 emu");

	sf::Texture screen_bkp_txt;
	screen_bkp_txt.create(window.getSize().x, window.getSize().y);
	sf::Sprite screen_bkp(screen_bkp_txt);

	for (unsigned int i = 0; i < 64*32; i++) {
		pixels[i].setSize(sf::Vector2f(10.f,10.f));
		//pixels[i].setPosition((((i%8)+(8*(i/(32*8)))) * 10), (((i % (8*32)) / 8) * 10));
		pixels[i].setPosition((i%64)*10, (i/64)*10);
		pixels[i].setFillColor(sf::Color::Black);
	}

	while (window.isOpen()) {
		//std::cout << *cpu.keys << std::endl;
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			} else if (event.type == sf::Event::KeyPressed) {
				switch (event.key.code) {
					case sf::Keyboard::Num4:
						*cpu.keys |= 1;
						//std::cout << *cpu.keys << std::endl;
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
		if (cpu.update_screen) {
			update_and_draw_pixels(cpu.gfx, pixels, &window);
			screen_bkp_txt.update(window);
		} else {
			window.draw(screen_bkp);
		}
		window.display();

		//sf::sleep(sf::milliseconds(10));
	}
	return 0;
}