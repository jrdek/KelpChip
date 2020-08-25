#ifndef C8_CPU
#define C8_CPU

#include <SFML/System/Clock.hpp>

// referencing multigesture.net's guide

class chip8 {
public:
/* METHODS */

	// constructor (no dynamic malloc means no destructor needed)
	chip8();

	// load a program
	int load_program(char* filename);

	// execute the instruction at the PC
	void exec_next(); 

	// disassembly (REMOVE ME)
	void disas_op(unsigned short op);

/* ATTRIBUTES */

	// memory runs from 0x000 to 0xFFF
	unsigned char mem[4096];

	// 8-bit regs V0 through VE, along with the flag reg VF
	unsigned char* V;

	// index register I and program counter PC
	unsigned short* I;
	unsigned short* PC;

	// two timers, to be dec'd every frame
	unsigned char* delay_timer;
	unsigned char* sound_timer;
	// and something to keep them honest...
	sf::Clock limiter_clock;

	// one **bit** per pixel; 64x32 display.
	// So this is a 256-byte long section of mem... ideally.
	//unsigned char* gfx;
	unsigned char gfx[64*32];

	// same, but prepped for on-screen display by sfml!
	unsigned char gfx_colors[64*32*4];  // rgba

	// stack needed for call/ret
	unsigned short* stack;
	unsigned char* SP;

	// keys currently pressed
	unsigned short* keys;
	// keys that were pressed last cycle
	unsigned short* old_keys;
	bool update_screen;

private:

	// draw a sprite...
	void draw_sprite(unsigned char x, unsigned char y, unsigned char n);
	unsigned char get_new_key();
};

#endif