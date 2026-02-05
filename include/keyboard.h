#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <SDL2/SDL.h>
#include "synth.h"

/*
 * Get the keyboard input from the SDL key event and the keyboard layout (QWERTY or AZERTY)
 * Change the synth_t voices frequencies with the assigned note
 * Change the ADSR parameters when the assigned keys are being pressed
 * Change the cutoff, detune and amplification when the assigned keys are being pressed
 * Change the keyboard octave when UP or DOWN keys are being pressed
 */
void handle_input(SDL_Keycode key, synth_t *synth, int layout, int *octave,
                  double *attack, double *decay, double *sustain, double *release);

/* Free the synth voices when their assigned note key are being released */
void handle_release(SDL_Keycode key, synth_t *synth, int layout, int octave);

/*
 * Converts a given key with its keyboard layout to a MIDI note
 * Returns -1 when the key is not assigned to a note, and the MIDI note otherwise
 */
int key_to_note(SDL_Keycode key, int kb_layout, int octave);

#endif