#ifndef INTERFACE_H
#define INTERFACE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "synth.h"

/**
 * Structure for the cached text SDL textures
 */
typedef struct
{
    SDL_Texture *title_texture;
    SDL_Texture *envelope_texture;
    SDL_Texture *waveform_texture;
    SDL_Texture *parameters_texture;
    char last_title_text[256];
    char last_envelope_text[256];
    char last_waveform_text[256];
    char last_parameters_text[256];
} text_cache_t;

/**
 * Render the synth basic informations into the SDL renderer with the given font :
 * - ADSR envelope parameters
 * - Voices oscillators waveforms
 * - Filter cutoff
 * - Detune coefficient
 * - Amplification coefficient
 */
void render_infos(synth_t synth, TTF_Font *font, SDL_Renderer *renderer,
                  double attack, double decay, double sustain, double release);

/**
 * Cleanup function for the cached text texture
 */
void cleanup_text_cache();

/**
 * Renders the waveform generated from the render_synth function into the SDL renderer
 */
void render_waveform(SDL_Renderer *renderer, short *buffer);

/**
 * Render the white keys from the MIDI piano visualizer into the SDL renderer
 */
void render_white_keys(SDL_Renderer *renderer);

/**
 * Render the black keys from the MIDI piano visualizer into the SDL renderer
 */
void render_black_keys(SDL_Renderer *renderer);

/**
 * Renders given note into a pressed key in the MIDI piano visualizer
 */
void render_key(SDL_Renderer *renderer, int midi_note);

/**
 * Returns a given MIDI note rectangle parameters (x, y, width and height)
 * Used to draw the piano keys into the SDL renderer
 */
void get_key_position(int midi_note, int *x, int *y,
                      int *width, int *height, int *is_black);

/**
 * Returns if a MIDI note is a assigned to a black key or not
 */
int is_black_key(int midi_note);

#endif