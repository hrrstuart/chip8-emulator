#define SDL_MAIN_HANDLED

#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "chip8.h"
#include "display.h"

int main(int argc, char *argv[]) {
    Chip8 chip8;
    chip8_init(&chip8);
    srand(time(NULL));
    
    // Get SDL2 renderer
    SDLContext sdl_context;
    open_window(&sdl_context);

    LARGE_INTEGER freq, start, now;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&start);
    int i = 0;
    while (1) {
        chip8_cycle(&chip8);

        if (chip8.draw_flag) {
            draw_display(&chip8, sdl_context.renderer);
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