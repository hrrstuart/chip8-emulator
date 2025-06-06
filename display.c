#include <SDL2/SDL.h>
#include "chip8.h"
#include "display.h"
#define SCALE 10

void open_window(SDLContext* sdl_context) {
    SDL_Init(SDL_INIT_VIDEO);
    sdl_context->window = SDL_CreateWindow("CHIP-8 Emulator",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        640, 320,
        SDL_WINDOW_SHOWN
    );
    sdl_context->renderer = SDL_CreateRenderer(sdl_context->window, -1, SDL_RENDERER_ACCELERATED);

    return;
}

void draw_display(Chip8* chip8, SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < sizeof(chip8->display); i++) {
        if (chip8->display[i]) {
            int x = i % 64;
            int y = i / 64;
            SDL_Rect rect = { x*SCALE, y*SCALE, SCALE, SCALE };
            SDL_RenderFillRect(renderer, &rect);
        }   
    }

    SDL_RenderPresent(renderer);
}