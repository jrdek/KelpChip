#include "chip8.h"  // cpu core header
#include <cstdlib>
#include <ctime>
#include <stdlib.h>
#include <iostream>
#include <fstream>

chip8::chip8() {

	// allow rng to not suck
	srand(time(NULL));

	// sprites, to be loaded into memory
	unsigned char sprites[] = {
		0xf0, 0x90, 0x90, 0x90, 0xf0,  // 0
		0x20, 0x60, 0x20, 0x20, 0x70,  // 1
		0xf0, 0x10, 0xf0, 0x80, 0xf0,  // 2
		0xf0, 0x10, 0xf0, 0x10, 0xf0,  // 3
		0x90, 0x90, 0xf0, 0x10, 0x10,  // 4
		0xf0, 0x80, 0xf0, 0x10, 0xf0,  // 5
		0xf0, 0x80, 0xf0, 0x90, 0xf0,  // 6
		0xf0, 0x10, 0x20, 0x40, 0x40,  // 7
		0xf0, 0x90, 0xf0, 0x90, 0xf0,  // 8
		0xf0, 0x90, 0xf0, 0x10, 0xf0,  // 9
		0xf0, 0x90, 0xf0, 0x90, 0x90,  // A
		0xe0, 0x90, 0xe0, 0x90, 0xe0,  // B
		0xf0, 0x80, 0x80, 0x80, 0xf0,  // C
		0xe0, 0x90, 0x90, 0x90, 0xe0,  // D
		0xf0, 0x80, 0xf0, 0x80, 0xf0,  // E
		0xf0, 0x80, 0xf0, 0x80, 0x80   // F
	};

	int i = 0;

	// 0x000 - 0x04F: sprites
	while (i < 0x50) {
		mem[i] = sprites[i];  // first 0x50 bytes in memory: sprites!
		i++;
	}

	// 0x050 - 0x14F: gfx... but to be honest, this makes sprite drawing kind of difficult. Time to cheat.
	//gfx = mem+0x50;
	for (int j = 0; j < 64*32; j++) {
		gfx[j] = 0;
	}

	// 0x150 - 0x151: keys pressed
	keys = (unsigned short*) (mem+0x150);

	// 0x152 - 0x153: old keys pressed
	old_keys = (unsigned short*) (mem+0x152);

	// 0x154 - 0x15F: (empty)
	
	// 0x160 - 0x16F: registers
	V = mem+0x160;

	// 0x170 - 0x171: I
	I = (unsigned short*) (mem+0x170);

	// 0x172 - 0x173: PC
	PC = (unsigned short*) (mem+0x172);

	// 0x174: delay timer
	delay_timer = mem+0x174;

	// 0x175: sound timer
	sound_timer = mem+0x175;

	// 0x176 - 0x177: SP
	SP = mem+0x176;

	// 0x178 upward: stack
	stack = (unsigned short*) (mem+0x178);

	// initialize memory beyond the sprites to 0
	while (i < 0x1000) {
		mem[i] = 0;
		i++;
	}

	*delay_timer = 1;
	*sound_timer = 1;  // just to avoid buzzing, we'll set the timers to 1 instead of 0
	*PC = 0x200;
	update_screen = false;

}


int chip8::load_program(char* filename) {
	/*
	1. attempt to read the file
	2. write every byte in the file to mem, starting at 0x200 and going up
		(note that chip8 is big-endian)
	*/
	std::ifstream prog = std::ifstream(filename, std::ios::binary);
	if (!(prog.is_open())) {
		printf("ERROR: couldn't read file.\n");
		return 1;
	}
	printf("Now reading %s...\n", filename);

	// get file size
	std::streampos begin, end;
	begin = prog.tellg();
	prog.seekg(0, std::ios::end);
	end = prog.tellg();
	unsigned int size = (unsigned int) (end - begin);
	prog.seekg(0, std::ios::beg);

	char c = 0x1;
	unsigned int i = 0;
	while (i < size) {
		c = prog.get();
		mem[0x200+i] = c;
		i++;
	}
	printf("Loaded %d bytes.\n", i);
	return 0;
}


void chip8::draw_sprite(unsigned char x, unsigned char y, unsigned char n) {
	// once again, god i hope this works
	unsigned short loc = (64*(y%32)) + (x%64);
	unsigned char byte = 0x00;
	for (int byte_num = 0; byte_num < n; byte_num++) {
		byte = mem[*I+byte_num];
		for (int bit = 0; bit < 8; bit++) {
			gfx[(64*(y%32)) + ((x+bit)%64)] ^= (byte >> (7-bit)) & 0x1;
			if (V[0xF] == 0) {
				V[0xF] = ((gfx[(64*(y%32)) + ((x+bit)%64)] == 0) && (((byte >> (7-bit)) & 0x1) == 1));
			}
		}
		y += 1;
	}
}


unsigned char chip8::get_new_key() {
	unsigned char ans = 0;
	unsigned short check_me = *keys ^ *old_keys;
	for (int i = 0; i < 16; i++) {
		if (check_me & 0x1) {
			return i;
		}
		check_me >>= 1;
	}
	printf("ERROR: Unexpected behavior in get_new_key()\n");
	return 0x0;
}


void chip8::exec_next() {
	update_screen = false;
	unsigned short opcode = (unsigned short) ((mem[*PC] << 8) | mem[*PC+1]);
	//disas_op(opcode);


	switch (opcode >> 12) {
		case 0: 
			if (opcode == 0x00e0) {
				// clear screen
				//for (int i = 0; i < 256; i++) {
				for (int i = 0; i < 64*32; i++) {
					gfx[i] = 0x00;
					update_screen = true;
				}
			} else if (opcode == 0x00ee) {
				// return
				if (*SP == 0) {  // By convention, we're going to say that the stack has one empty spot in it.
					printf("ERROR: ret from empty stack\n");
				}
				*PC = stack[*SP] - 2;
				*SP = *SP - 1;
			} else {
				// sys; not supported
				printf("ERROR: sys command not implemented\n");
				//printf("sys %x%x%x", (opcode>>8)&0xF, (opcode>>4)&0xF, (opcode)&0xF);
			}
			break;

		case 1:
			// unconditional jump
			*PC = (opcode & 0xFFF) - 2;
			break;

		case 2:
			// call a subroutine
			*SP = *SP + 1;
			if (*SP == 0x0) {
				printf("ERROR: stack overflow\n");
			}
			stack[*SP] = *PC;
			*PC = (opcode & 0xFFF) - 2;
			break;

		case 3:
			// skip next instruction if reg == imm
			if (V[(opcode>>8)&0xF] == (opcode & 0xFF)) {
				*PC += 2;
			}
			break;

		case 4:
			// skip next instruction if reg != imm
			if (V[(opcode>>8)&0xF] != (opcode & 0xFF)) {
				*PC += 2;
			}
			break;

		case 5:
			// skip next instruction if reg == reg
			if (V[(opcode>>8)&0xF] == V[(opcode>>4)&0xF]) {
				*PC += 2;
			}
			break;

		case 6:
			// load value into a register
			V[(opcode>>8)&0xF] = (unsigned char) (opcode & 0xFF);
			break;

		case 7:
			// add value to a register
			V[(opcode>>8)&0xF] += (unsigned char) (opcode & 0xFF);
			break;

		case 8:
			switch (opcode & 0xF) {
				case 0:
					// load one register's value into another
					V[(opcode>>8)&0xF] = V[(opcode>>4)&0xF];
					break;
				case 1:
					// OR
					V[(opcode>>8)&0xF] |= V[(opcode>>4)&0xF];
					break;
				case 2:
					// AND
					V[(opcode>>8)&0xF] &= V[(opcode>>4)&0xF];
					break;
				case 3:
					// XOR
					V[(opcode>>8)&0xF] ^= V[(opcode>>4)&0xF];
					break;
				case 4:
					// add
					V[(opcode>>8)&0xF] += V[(opcode>>4)&0xF];
					V[0xF] = (V[(opcode>>8)&0xF] < V[(opcode>>4)&0xF]);
					break;
				case 5:
					// subtract
					V[0xF] = (V[(opcode>>8)&0xF] > V[(opcode>>4)&0xF]);
					V[(opcode>>8)&0xF] -= V[(opcode>>4)&0xF];
					break;
				case 6:
					// shift right by 1
					V[0xF] = (V[(opcode>>8)&0xF] & 0x1);
					V[(opcode>>8)&0xF] >>= 1;
					break;
				case 7:
					// "subn", which is basically the opposite of subtract
					V[0xF] = (V[(opcode>>8)&0xF] < V[(opcode>>4)&0xF]);
					V[(opcode>>8)&0xF] = V[(opcode>>4)&0xF] - V[(opcode>>8)&0xF];
					break;
				case 14:
					// shift left by 1
					V[0xF] = !!(V[(opcode>>8)&0xF] & 0x8000);
					V[(opcode>>8)&0xF] <<= 1;
					break;
			}
			break;

		case 9:
			// skip next instruction if reg != reg
			if (V[(opcode>>8)&0xF] != V[(opcode>>4)&0xF]) {
				*PC += 2;
			}
			break;

		case 10:
			// load value into I
			*I = (opcode & 0xFFF);
			break;

		case 11:
			// unconditional jump to V0 + nnn
			*PC = V[0] + (opcode & 0xFFF) - 2;
			break;

		case 12:
			// generate a random byte, AND it with an imm, and store the result in a reg
			V[(opcode>>8)&0xF] = (unsigned char) ((opcode & 0xFF) & (rand() % 256));
			break;

		case 13:
			// Draw an n-byte sprite at Vx, Vy...
			draw_sprite(V[(opcode>>8)&0xF], V[(opcode>>4)&0xF], (unsigned char) (opcode)&0xF);
			update_screen = true;
			break;

		case 14:
			if ((opcode & 0xF) == 0xE) {
				// skip the next instruction if a particular key is pressed
				if (keys[V[(opcode>>8)&0xF]]) {
					*PC += 2;
				}
			} else {
				// skip the next instruction if a particular key is not pressed
				if (!(keys[V[(opcode>>8)&0xF]])) {
					*PC += 2;
				}
			}
			break;

		case 15:
			switch (opcode & 0xFF) {
				case 0x07:
					// load the delay timer into a register
					V[(opcode>>8)&0xF] = *delay_timer;
					break;
				case 0x0a:
					// TODO: wait for a key press, then store the pressed key in a reg
					if (*old_keys < *keys) {
						V[(opcode>>8)&0xF] = get_new_key();
					} else {
						*PC -= 2;  // "halt execution". Really, just get stuck here.
					}
					break;
				case 0x15:
					// set the delay timer from a register
					*delay_timer = V[(opcode>>8)&0xF];
					break;
				case 0x18:
					// set the sound timer from a register
					*sound_timer = V[(opcode>>8)&0xF];
					break;
				case 0x1e:
					// add a register to I
					*I += V[(opcode>>8)&0xF];
					break;
				case 0x29:
					// set I to the address of the sprite of the digit in Vx
					// (NOTE: This will behave very strangely when Vx > 0xF)
					if (V[(opcode>>8)&0xF] > 0xF) {
						printf("ERROR: ldspr on a non-digit\n");
					}
					*I = 5*V[(opcode>>8)&0xF];
					break;
				case 0x33:
					// store binary coded decimal of Vx in mem[I], mem[I+1], mem[I+2]
					mem[*I] = V[(opcode>>8)&0xF] / 100;
					mem[*I+1] = (V[(opcode>>8)&0xF] % 100) / 10;
					mem[*I+2] = V[(opcode>>8)&0xF] % 10;
					break;
				case 0x55:
					// store registers V0 through Vx in memory, starting at I
					for (int i = 0; i <= ((opcode>>8)&0xF); i++) {
						mem[*I+i] = V[i];
					}
					break;
				case 0x65:
					// load registers V0 through Vx from memory, starting at I
					for (int i = 0; i <= ((opcode>>8)&0xF); i++) {
						V[i] = mem[*I+i];
					}
					break;
			}
			break;
	}

	*PC += 2;
	if (*delay_timer) {
		*delay_timer -= 1;
	}
	if (*sound_timer) {
		*sound_timer -= 1;
	}
	*old_keys = *keys;
}


void chip8::disas_op(unsigned short opcode) {
	switch (opcode >> 12) {
		case 0: 
			if (opcode == 0x00e0) {
				printf("cls");
			} else if (opcode == 0x00ee) {
				printf("ret");
			} else {
				printf("sys %x%x%x", (opcode>>8)&0xF, (opcode>>4)&0xF, (opcode)&0xF);
			}
			break;

		case 1:
			printf("jp 0x%x%x%x", (opcode>>8)&0xF, (opcode>>4)&0xF, (opcode)&0xF);
			break;

		case 2:
			printf("call 0x%x%x%x", (opcode>>8)&0xF, (opcode>>4)&0xF, (opcode)&0xF);
			break;

		case 3:
			printf("se V%x, $0x%x%x", (opcode>>8)&0xF, (opcode>>4)&0xF, (opcode)&0xF);
			break;

		case 4:
			printf("sne V%x, $0x%x%x", (opcode>>8)&0xF, (opcode>>4)&0xF, (opcode)&0xF);
			break;

		case 5:
			printf("se V%x, V%x", (opcode>>8)&0xF, (opcode>>4)&0xF);
			break;

		case 6:
			printf("ld V%x, $0x%x%x", (opcode>>8)&0xF, (opcode>>4)&0xF, (opcode)&0xF);
			break;

		case 7:
			printf("add V%x, $0x%x%x", (opcode>>8)&0xF, (opcode>>4)&0xF, (opcode)&0xF);
			break;

		case 8:
			switch (opcode & 0xF) {
				case 0:
					printf("ld V%x, V%x", (opcode>>8)&0xF, (opcode>>4)&0xF);
					break;
				case 1:
					printf("or V%x, V%x", (opcode>>8)&0xF, (opcode>>4)&0xF);
					break;
				case 2:
					printf("and V%x, V%x", (opcode>>8)&0xF, (opcode>>4)&0xF);
					break;
				case 3:
					printf("xor V%x, V%x", (opcode>>8)&0xF, (opcode>>4)&0xF);
					break;
				case 4:
					printf("add V%x, V%x", (opcode>>8)&0xF, (opcode>>4)&0xF);
					break;
				case 5:
					printf("sub V%x, V%x", (opcode>>8)&0xF, (opcode>>4)&0xF);
					break;
				case 6:
					printf("shr  V%x, V%x", (opcode>>8)&0xF, (opcode>>4)&0xF);
					break;
				case 7:
					printf("subn V%x, V%x", (opcode>>8)&0xF, (opcode>>4)&0xF);
					break;
				case 14:
					printf("shl V%x, V%x", (opcode>>8)&0xF, (opcode>>4)&0xF);
					break;
			}
			break;

		case 9:
			printf("sne V%x, V%x", (opcode>>8)&0xF, (opcode>>4)&0xF);
			break;

		case 10:
			printf("ld I, %x%x%x", (opcode>>8)&0xF, (opcode>>4)&0xF, (opcode)&0xF);
			break;

		case 11:
			printf("jp V0 + $0x%x%x%x", (opcode>>8)&0xF, (opcode>>4)&0xF, (opcode)&0xF);
			break;

		case 12:
			printf("rnd V%x, $0x%x%x", (opcode>>8)&0xF, (opcode>>4)&0xF, (opcode)&0xF);
			break;

		case 13:
			printf("drw V%x, V%x, $0x%x", (opcode>>8)&0xF, (opcode>>4)&0xF, (opcode)&0xF);
			break;

		case 14:
			if ((opcode & 0xF) == 0xE) {
				printf("skp V%x", (opcode>>8)&0xF);
			} else {
				printf("sknp V%x", (opcode>>8)&0xF);
			}
			break;

		case 15:
			switch (opcode & 0xFF) {
				case 0x07:
					printf("ld V%x, DT", (opcode>>8)&0xF);
					break;
				case 0x0a:
					printf("ld V%x, K", (opcode>>8)&0xF);
					break;
				case 0x15:
					printf("ld DT, V%x", (opcode>>8)&0xF);
					break;
				case 0x18:
					printf("ld ST, V%x", (opcode>>8)&0xF);
					break;
				case 0x1e:
					printf("add I, V%x", (opcode>>8)&0xF);
					break;
				case 0x29:
					printf("ldspr V%x", (opcode>>8)&0xF);
					break;
				case 0x33:
					printf("ldbcd V%x", (opcode>>8)&0xF);
					break;
				case 0x55:
					printf("ldtil [I], V%x", (opcode>>8)&0xF);
					break;
				case 0x65:
					printf("ldtil V%x, [I]", (opcode>>8)&0xF);
					break;
			}
			break;
	}
	printf("\n");
}