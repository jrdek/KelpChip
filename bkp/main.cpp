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

	for (unsigned int i = 0; i < 64*32; i++) {
		pixels[i].setSize(sf::Vector2f(10.f,10.f));
		//pixels[i].setPosition((((i%8)+(8*(i/(32*8)))) * 10), (((i % (8*32)) / 8) * 10));
		pixels[i].setPosition((i%64)*10, (i/64)*10);
		pixels[i].setFillColor(sf::Color::Black);
	}

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}

		// run the next step
		cpu.exec_next();
		if (cpu.update_screen) {
			window.clear();
			update_and_draw_pixels(cpu.gfx, pixels, &window);
		}
		window.display();

		//sf::sleep(sf::milliseconds(10));
	}
	return 0;
}