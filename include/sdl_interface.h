#ifndef SDL_INTERFACE_H
#define SDL_INTERFACE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "synth.h"
#include "defs.h"

#define WIDTH 1200
#define HEIGHT 600

void render_interface(poly_synth_t synth, TTF_Font *font, SDL_Renderer *renderer);
void render_waveform(SDL_Renderer *renderer, short *buffer);

#endif 