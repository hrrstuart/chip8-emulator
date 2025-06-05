#ifndef CHIP8_H
#define CHIP8_H

typedef struct {
    unsigned char memory[4096];
    unsigned char V[16];
    unsigned short pc;
    unsigned short I;
    unsigned short stack[16];
    unsigned char display[64*32];
} Chip8;

int chip8_init(Chip8* chip8);

#endif