#include <SDL2/SDL.h>

#include "kb_input.h"
#include "defs.h"
#include "synth.h"

/**
 * Function that read keyboard input from the SDL_Event sent by the user
 * Into a note_t struct that is then used to actualize the frequency of the synth_3osc_t oscillators
 */
void handle_input(SDL_Event *event, note_t *note, synth_3osc_t *synth) {

    SDL_Keycode key = event->key.keysym.sym;

    switch(key) {
        case kC:
            note->semitone = nC;
            change_osc_freq(synth, *note);
            break;
        case kC_SHARP:
            note->semitone = nC_SHARP;
            change_osc_freq(synth, *note);
            break;
        case kD:
            note->semitone = nD;
            change_osc_freq(synth, *note);
            break;
        case kD_SHARP:
            note->semitone = nD_SHARP;
            change_osc_freq(synth, *note);
            break;
        case kE:
            note->semitone = nE;
            change_osc_freq(synth, *note);
            break;
        case kF:
            note->semitone = nF;
            change_osc_freq(synth, *note);
            break;
        case kF_SHARP:
            note->semitone = nF_SHARP;
            change_osc_freq(synth, *note);
            break;
        case kG:
            note->semitone = nG;
            change_osc_freq(synth, *note);
            break;
        case kG_SHARP:
            note->semitone = nG_SHARP;
            change_osc_freq(synth, *note);
            break;
        case kA:
            note->semitone = nA;
            change_osc_freq(synth, *note);
            break;
        case kA_SHARP:
            note->semitone = nA_SHARP;
            change_osc_freq(synth, *note);
            break;
        case kB:
            note->semitone = nB;
            change_osc_freq(synth, *note);
            break;
        case SDLK_UP:
            note->octave++;
            break;
        case SDLK_DOWN:
            note->octave--;
            break;
        case OSC_A_WAVE_INCREMENT:
            synth->osc_a->wave = (synth->osc_a->wave + 1) % 4;
            break;
        case OSC_B_WAVE_INCREMENT:
            synth->osc_b->wave = (synth->osc_b->wave + 1) % 4;
            break;
        case OSC_C_WAVE_INCREMENT:
            synth->osc_c->wave = (synth->osc_c->wave + 1) % 4;
            break;
        case ATTACK_INCREMENT:
            synth->adsr->att += 0.05;
            if (synth->adsr->att > 1.05) synth->adsr->att = 0.0;
            break;
        case DECAY_INCREMENT:
            synth->adsr->dec += 0.05;
            if (synth->adsr->dec > 1.05) synth->adsr->dec = 0.0;
            break;
        case SUSTAIN_INCREMENT:
            synth->adsr->sus += 0.05;
            if (synth->adsr->sus > 1.05) synth->adsr->sus = 0.0;
            break;
        case RELEASE_INCREMENT:
            synth->adsr->rel += 0.05;
            if (synth->adsr->rel > 1.05) synth->adsr->rel = 0.0;
            break;
        case DETUNE_INCREMENT:
            synth->detune += 0.05;
            if (synth->detune > 1.05) synth->detune = 0.0;
            break;
        default:
            break;
        }

}