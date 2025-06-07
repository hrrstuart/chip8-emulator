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

    LARGE_INTEGER freq, start, now, timer_last;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&start);
    timer_last = start;

    int i = 0;
    while (1) {
        int program_exit = chip8_cycle(&chip8);
        if (program_exit) {
            break;
        }

        if (chip8.draw_flag) {
            draw_display(&chip8, sdl_context.renderer);
            chip8.draw_flag = 0;
        }

        // 60 Hz timer update
        LARGE_INTEGER timer_now;
        QueryPerformanceCounter(&timer_now);
        double timer_elapsed = (double)(timer_now.QuadPart - timer_last.QuadPart) * 1000 / freq.HighPart;
        if (timer_elapsed >= 16.67) {
            if (chip8.delay_timer > 0) chip8.delay_timer--;
            if (chip8.sound_timer > 0) chip8.sound_timer--;
            timer_last = timer_now;
        }

        // 700 Hz instruction timing
        QueryPerformanceCounter(&now);
        double elapsed = (double)(now.QuadPart - start.QuadPart) * 1000.0 / freq.QuadPart;
        if (elapsed < 1.42857) {
            Sleep(0);
            continue;
        }
        start = now;
    }

    close_window(&sdl_context);

    return 0;
}