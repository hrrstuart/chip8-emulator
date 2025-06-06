#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "chip8.h"

int load_rom(Chip8* chip8, char* filename) {
    FILE* program = fopen(filename, "rb");
    if (!program) return 1;
    
    fseek(program, 0, SEEK_END);
    long file_size = ftell(program);
    rewind(program);
    
    size_t bytes_read = fread(chip8->memory + 0x200, 1, file_size, program);
    fclose(program);
    
    if (bytes_read != file_size) return 1;
    return 0;
}

int chip8_init(Chip8* chip8) {
    memset(chip8, 0, sizeof(Chip8));
    int rom_failed = load_rom(chip8, "./roms/ibm-logo.ch8");
    if (rom_failed) return 1;

    // Chip8 programs start at memory address 0x200
    chip8->pc = 0x200;

    char font[5*16] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    // Load font into 0x50 to 0x9F memory slots
    int start_address = 0x50;
    memcpy(chip8->memory + start_address, font, sizeof(font));

    return 0;
}

// Set the CHIP-8 display array to new values
int configure_display(Chip8* chip8, uint8_t x, uint8_t y, uint8_t n) {
    uint8_t x_coord = chip8->V[x] % 64;
    uint8_t y_coord = chip8->V[y] % 32;
    chip8->V[0xF] = 0;

    for (int y = 0; y < n; y++) {
        uint8_t sprite_row = chip8->memory[chip8->I + y];
        x_coord = chip8->V[x] % 64;
        for (int i = 0; i < 8; i++) {
            int bit = (sprite_row >> (7-i)) & 1;
            int index = (y_coord%32) * 64 + (x_coord % 64);
            if (bit == 1) {
                uint8_t already_on = chip8->display[index];
                if (already_on) {
                    chip8->display[index] = 0;
                    chip8->V[0xF] = 1;
                } else {
                    chip8->display[index] = 1;
                }
            }
            x_coord += 1;
            if (x_coord >= 64) break;
        }
        y_coord += 1;
        if (y_coord >= 32) break;
    }

    return 0; // Not implemented error
}

uint16_t fetch_memory(Chip8* chip8) {
    uint8_t byte_one = chip8->memory[chip8->pc];
    uint8_t byte_two = chip8->memory[chip8->pc + 1];
    chip8->pc += 2;
    
    return byte_one<<8|byte_two;
}

int decode_and_execute(uint16_t instruction, Chip8* chip8) {
    // Split into nibbles
    uint8_t x = (instruction & 0x0F00) >> 8; // 2nd nibble
    uint8_t y = (instruction & 0x00F0) >> 4; // 3rd nibble
    uint8_t n = (instruction & 0x000F); // 4th nibble
    uint8_t nn = (instruction & 0x00FF); // 3rd and 4th nibble
    uint16_t nnn = (instruction & 0x0FFF); // 2nd, 3rd, and 4th nibble

    // What to do based on first nibble
    switch ((instruction & 0xF000)>>12) {
        case 0x0: // 00E0
            if (nnn == 0x0E0) {
                memset(chip8->display, 0, sizeof(chip8->display));
                chip8->draw_flag = 1;
            } else if (nnn == 0x0EE) {
                chip8->sp--;
                chip8->pc = chip8->stack[chip8->sp];
            }
            break;
        case 0x1: // Set pc to nnn
            chip8->pc = nnn;
            break;
        case 0x2: // Call subroutine at CHIP-8 memory location nnn
            chip8->stack[chip8->sp] = chip8->pc;
            chip8->sp++;
            chip8->pc = nnn;
            break;
        case 0x3: // If VX == nn, skip one instruction
            if (chip8->V[x] == nn) {
                chip8->pc += 2;
            }
            break;
        case 0x4: // If VX != nn, skip one instruction
            if (chip8->V[x] != nn) {
                chip8->pc += 2;
            }
            break;
        case 0x5: // If VX == VY, skip one instruction
            if (chip8->V[x] == chip8->V[y]) {
                chip8->pc += 2;
            }
            break;
        case 0x9: // If VX != VY, skip one instruction
            if (chip8->V[x] != chip8->V[y]) {
                chip8->pc += 2;
            }
            break;
        case 0x6: // Set VX to nn
            chip8->V[x] = nn;
            break;
        case 0x7: // Add nn to VX
            chip8->V[x] += nn;
            break;
        case 0xA:
            chip8->I = nnn;
            break;
        case 0xD:
            configure_display(chip8, x, y, n);
            chip8->draw_flag = 1;
            break;
    }
    return 0;
}

int chip8_cycle(Chip8* chip8) {
    uint16_t instruction = fetch_memory(chip8);
    return decode_and_execute(instruction, chip8);
}