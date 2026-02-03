#ifndef KEYBOARD_INPUT_H
#define KEYBOARD_INPUT_H

#include <SDL2/SDL.h>
#include "defs.h"
#include "synth.h"

#define QWERTY 0
#define AZERTY 1

// NOTE KEYS
#define kC_QWERTY SDLK_a
#define kC_AZERTY SDLK_q
#define kC_SHARP_QWERTY SDLK_w
#define kC_SHARP_AZERTY SDLK_z
#define kD SDLK_s
#define kD_SHARP SDLK_e
#define kE SDLK_d
#define kF SDLK_f
#define kF_SHARP SDLK_t
#define kG SDLK_g
#define kG_SHARP SDLK_y
#define kA SDLK_h
#define kA_SHARP SDLK_u
#define kB SDLK_j

#define OSC_A_WAVE_INCREMENT SDLK_1
#define OSC_B_WAVE_INCREMENT SDLK_2
#define OSC_C_WAVE_INCREMENT SDLK_3

#define DETUNE_INCREMENT SDLK_4
#define CUTOFF_INCREMENT SDLK_5

#define ATTACK_INCREMENT_QWERTY SDLK_z
#define ATTACK_INCREMENT_AZERTY SDLK_w
#define DECAY_INCREMENT SDLK_x
#define SUSTAIN_INCREMENT SDLK_c
#define RELEASE_INCREMENT SDLK_v

void handle_input(SDL_Event *event, note_t *note, poly_synth_t *synth, int kb_layout, int *n_voices);
void handle_release(SDL_Event *event, note_t *note, poly_synth_t *synth, int kb_layout, int *n_voices);
int key_to_note(SDL_Keycode key, note_t *note, int kb_layout);

#endif