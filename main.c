#define SDL_MAIN_HANDLED

#include <stdio.h>
#include <windows.h>
#include <SDL2/SDL.h>
#include "chip8.h"
#include "display.h"

int main(int argc, char *argv[]) {
    Chip8 chip8;
    chip8_init(&chip8);
    
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("CHIP-8 Emulator",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        640, 320,
        SDL_WINDOW_SHOWN
    );
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    LARGE_INTEGER freq, start, now;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&start);
    int i = 0;
    while (1) {
        chip8_cycle(&chip8);

        if (chip8.draw_flag) {
            draw_display(&chip8, renderer);
            chip8.draw_flag = 0;
        }

        QueryPerformanceCounter(&now);
        double elapsed = (double)(now.QuadPart - start.QuadPart) * 1000.0 / freq.QuadPart;

        if (elapsed < 1.42857) {
            Sleep(0);
            continue;
        }
        start = now;
    }

    return 0;
}