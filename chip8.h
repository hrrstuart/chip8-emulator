#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

typedef struct {
    uint8_t memory[4096];
    uint16_t V[16];
    uint16_t pc;
    uint8_t sp;
    uint16_t I;
    uint16_t stack[16];
    uint8_t display[64*32];
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint8_t draw_flag;
} Chip8;

int chip8_init(Chip8* chip8);
int chip8_cycle(Chip8* chip8);

#endif