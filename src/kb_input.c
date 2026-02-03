#include <SDL2/SDL.h>

#include "kb_input.h"
#include "defs.h"
#include "synth.h"

/**
 * Function that read keyboard input from the SDL_Event sent by the user
 * Into a note_t struct that is then used to actualize the frequency of the synth_3osc_t oscillators
 */
void handle_input(SDL_Event *event, note_t *note, poly_synth_t *synth, int kb_layout, int *n_voices) {

    SDL_Keycode key = event->key.keysym.sym;

    int note_changed = key_to_note(key, note, kb_layout);

    if (note_changed && *n_voices < 6) {
        (*n_voices)++;
        synth_3osc_t *unused_voice = get_unused_voice(synth);
        if (unused_voice == NULL) return;
        change_osc_freq(unused_voice, *note, synth->detune);
        return;
    } else if (note_changed && *n_voices >= 6) {
        return;
    }

    adsr_t *ptr_adsr = synth->voice_a->adsr;
    
    switch(key) {
        case SDLK_UP:
            note->octave++;
            break;
        case SDLK_DOWN:
            note->octave--;
            break;
        case OSC_A_WAVE_INCREMENT:
            synth->voice_a->osc_a->wave = (synth->voice_a->osc_a->wave + 1) % 4;
            synth->voice_a->osc_b->wave = (synth->voice_a->osc_b->wave + 1) % 4;
            synth->voice_a->osc_c->wave = (synth->voice_a->osc_c->wave + 1) % 4;
            break;
        case OSC_B_WAVE_INCREMENT:
            synth->voice_b->osc_a->wave = (synth->voice_b->osc_a->wave + 1) % 4;
            synth->voice_b->osc_b->wave = (synth->voice_b->osc_b->wave + 1) % 4;
            synth->voice_b->osc_c->wave = (synth->voice_b->osc_c->wave + 1) % 4;
            break;
        case OSC_C_WAVE_INCREMENT:
            synth->voice_c->osc_a->wave = (synth->voice_c->osc_a->wave + 1) % 4;
            synth->voice_c->osc_b->wave = (synth->voice_c->osc_b->wave + 1) % 4;
            synth->voice_c->osc_c->wave = (synth->voice_c->osc_c->wave + 1) % 4;
            break;
        case ATTACK_INCREMENT_QWERTY:
            if (kb_layout == QWERTY) {
                ptr_adsr->att += 0.05;
                if (ptr_adsr->att > 1.05) ptr_adsr->att = 0.0;
            }
            break;
        case ATTACK_INCREMENT_AZERTY:
            if (kb_layout == AZERTY) {
                ptr_adsr->att += 0.05;
                if (ptr_adsr->att > 1.05) ptr_adsr->att = 0.0;
            }
            break;
        case DECAY_INCREMENT:
            ptr_adsr->dec += 0.05;
            if (ptr_adsr->dec > 1.05) ptr_adsr->dec = 0.0;
            break;
        case SUSTAIN_INCREMENT:
            ptr_adsr->sus += 0.05;
            if (ptr_adsr->sus > 1.05) ptr_adsr->sus = 0.0;
            break;
        case RELEASE_INCREMENT:
            ptr_adsr->rel += 0.05;
            if (ptr_adsr->rel > 1.05) ptr_adsr->rel = 0.0;
            break;
        case DETUNE_INCREMENT:
            synth->detune += 0.05;
            if (synth->detune > 1.05) synth->detune = 0.0;
            break;
        default:
            break;
        }

}

void handle_release(SDL_Event *event, note_t *note, poly_synth_t *synth, int kb_layout, int *n_voices) {
    SDL_Keycode key = event->key.keysym.sym;

    int note_released = key_to_note(key, note, kb_layout);

    if (note_released && *n_voices >= 1) {
        (*n_voices)--;
        int midi_note = note->octave * 12 + note->semitone;
        if (synth->voice_a->midi_note == midi_note)
            synth->voice_a->active = 0;
        if (synth->voice_b->midi_note == midi_note)
            synth->voice_b->active = 0;
        if (synth->voice_c->midi_note == midi_note)
            synth->voice_c->active = 0;
        if (synth->voice_d->midi_note == midi_note)
            synth->voice_d->active = 0;
        if (synth->voice_e->midi_note == midi_note)
            synth->voice_e->active = 0;
        if (synth->voice_f->midi_note == midi_note)
            synth->voice_f->active = 0;
    }
}

int key_to_note(SDL_Keycode key, note_t *note, int kb_layout) {
    int note_changed = 1;
    switch(key) {
        case kC_QWERTY:
            if (kb_layout == QWERTY)
                note->semitone = nC;
            else note_changed = 0;
            break;
        case kC_AZERTY:
            if (kb_layout == AZERTY)
                note->semitone = nC;
            else note_changed = 0;
            break;
        case kC_SHARP_QWERTY:
            if (kb_layout == QWERTY)
                note->semitone = nC_SHARP;
            else note_changed = 0;
            break;
        case kC_SHARP_AZERTY:
            if (kb_layout == AZERTY)
                note->semitone = nC_SHARP;
            else note_changed = 0;
            break;
        case kD:
            note->semitone = nD;
            break;
        case kD_SHARP:
            note->semitone = nD_SHARP;
            break;
        case kE:
            note->semitone = nE;
            break;
        case kF:
            note->semitone = nF;
            break;
        case kF_SHARP:
            note->semitone = nF_SHARP;
            break;
        case kG:
            note->semitone = nG;
            break;
        case kG_SHARP:
            note->semitone = nG_SHARP;
            break;
        case kA:
            note->semitone = nA;
            break;
        case kA_SHARP:
            note->semitone = nA_SHARP;
            break;
        case kB:
            note->semitone = nB;
            break;
        default:
            note_changed = 0;
            break;
    }
    return note_changed;

}
