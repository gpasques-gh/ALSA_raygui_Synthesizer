#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "sdl_interface.h"
#include "synth.h"

/**
 * SDL renderer for thetext interface 
 * Renders informations about the synthesizer and the note
 */
void render_interface(poly_synth_t synth, TTF_Font *font, SDL_Renderer *renderer) {
    SDL_Color black = {0, 0, 0, 255};
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "Voice A : %d | Voice B : %d | Voice C : %d | Voice D : %d | Voice E : %d | Voice F : %d", 
    synth.voice_a->midi_note, synth.voice_b->midi_note, synth.voice_c->midi_note, synth.voice_d->midi_note, synth.voice_e->midi_note, synth.voice_e->midi_note);
    SDL_Surface *surface = TTF_RenderText_Solid(font, buffer, black);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect surface_rect = {
        .h = 50,
        .w = WIDTH,
        .x = 0,
        .y = 0
    };
    SDL_RenderCopy(renderer, texture, NULL, &surface_rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    char buffer2[256];

    snprintf(buffer2, sizeof(buffer2), "Envelope - Attack: %.2f | Decay: %.2f | Sustain: %.2f | Release: %.2f", 
        synth.voice_a->adsr->att, synth.voice_a->adsr->dec, synth.voice_a->adsr->sus, synth.voice_a->adsr->rel);
    SDL_Surface *surface2 = TTF_RenderText_Solid(font, buffer2, black);
    SDL_Texture *texture2 = SDL_CreateTextureFromSurface(renderer, surface2);
    SDL_Rect surface_rect2 = {
        .h = 50,
        .w = WIDTH,
        .x = 0,
        .y = 60
    };
    SDL_RenderCopy(renderer, texture2, NULL, &surface_rect2);
    SDL_FreeSurface(surface2);
    SDL_DestroyTexture(texture2);

    char buffer3[256];

    snprintf(buffer3, sizeof(buffer3), "Waveforms - Voice A: %s | Voice B: %s | Voice C: %s", 
        get_wave_name(synth.voice_a->osc_a->wave), get_wave_name(synth.voice_b->osc_a->wave), get_wave_name(synth.voice_c->osc_a->wave));
    SDL_Surface *surface3 = TTF_RenderText_Solid(font, buffer3, black);
    SDL_Texture *texture3 = SDL_CreateTextureFromSurface(renderer, surface3);
    SDL_Rect surface_rect3 = {
        .h = 50,
        .w = WIDTH,
        .x = 0,
        .y = 120
    };
    SDL_RenderCopy(renderer, texture3, NULL, &surface_rect3);
    SDL_FreeSurface(surface3);
    SDL_DestroyTexture(texture3);
}

/**
 * Rendering the waveform from the waveform buffer
 */
void render_waveform(SDL_Renderer *renderer, short *buffer) {

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    int mid_y = HEIGHT / 2;
    
    for (int i = 0; i < FRAMES; i++) {
        int x1 = (i * WIDTH) / FRAMES;
        int x2 = ((i + 1) * WIDTH) / FRAMES;

        int y1 = mid_y - ((buffer[i] * mid_y) / 32768);
        int y2 = mid_y - ((buffer[i + 1] * mid_y) / 32768);

        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }
}