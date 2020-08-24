#include "disas.h"
#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char* argv[]) {
	if (argc < 2) {
		cout << "No file specified." << endl;
		return 1;
	}
	streampos size;

	ifstream rom_file (argv[1], ios::ate);  // open file with get pointer at end
	if (not rom_file.is_open()) {
		cout << "Couldn't read file." << endl;
		return 2;
	}

	size = rom_file.tellg() >> 1;
	if (not size) {
		cout << "Empty file." << endl;
		return 3;
	}

	unsigned short* rom = new unsigned short[((unsigned long) size)];
	rom_file.seekg(0, ios::beg);
	rom_file.read((char*)(&rom[0]), size*2);
	rom_file.close();

	cout << "Size: " << size << endl;

	// disassembly time!
	int pc = 0x0;
	while (pc < size) {
		cout << hex << ((2*pc)+0x200) << ": ";
		disas_op(rom, pc);
		pc += 1;
	}

	delete[] rom;

	return 0;
}