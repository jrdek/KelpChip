# KelpChip
A chip8 emulator written in C++ using SFML for graphics. Made for fun almost entirely in January 2020, then polished in August 2020.

Install SFML on Debian-based Linux with `sudo apt install libsfml-dev`, then run `make`. Run a ROM by typing `./kelpchip <filename>`.
The keypad is laid out with
```
4 5 6 7
R T Y U
F G H J
V B N M
```
corresponding to 
```
0 1 2 3
4 5 6 7
8 9 A B
C D E F
```

The `work` folder contains scratch work as well as roms. (Should I delete that from this repo?)
I used Cowgod's chip8 reference while creating this. Many, but not all, of the test .ch8 files are from dmatlack's chip8/roms repo (and thus, as far as I can tell, public domain). The others, and the annotations, are my own work.
