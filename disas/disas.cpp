#include <stdio.h>
#include "disas.h"

void disas_op(unsigned short* buff, int pc) {
	unsigned short* raw_code = buff + pc;
	unsigned short opcode = ((*raw_code << 8) | (*raw_code >> 8));
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