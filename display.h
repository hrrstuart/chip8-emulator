#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include "chip8.h"

typedef struct {
    SDL_Renderer* renderer;
    SDL_Window* window;
} SDLContext;


void open_window(SDLContext* sdl_context);
void close_window(SDLContext* sdl_context);
void draw_display(Chip8* chip8, SDL_Renderer* renderer);

#endif