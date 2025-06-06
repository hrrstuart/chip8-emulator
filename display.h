#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include "chip8.h"

void draw_display(Chip8* chip8, SDL_Renderer* renderer);

#endif