#ifndef MIDI_H
#define MIDI_H

#include <alsa/asoundlib.h>
#include "synth.h"

#define MIDI_MAX_VALUE 127.0

#define PRESSED 0xF0
#define NOTE_ON 0x90
#define NOTE_OFF 0x80
#define KNOB_TURNED 0xB0

// ADSR
#define ARTURIA_ATT_KNOB 73
#define ARTURIA_DEC_KNOB 75
#define ARTURIA_SUS_KNOB 79
#define ARTURIA_REL_KNOB 72

// WAVEFORMS
#define ARTURIA_OSC_A_KNOB 80
#define ARTURIA_OSC_B_KNOB 81
#define ARTURIA_OSC_C_KNOB 82
#define ARTURIA_OSC_D_KNOB 93
#define ARTURIA_OSC_E_KNOB 18
#define ARTURIA_OSC_F_KNOB 19

// EFFECTS
#define ARTURIA_CUTOFF_KNOB 74
#define ARTURIA_DETUNE_KNOB 71
#define ARTURIA_AMPLITUDE_KNOB 85


int get_midi(snd_rawmidi_t *midi_in, note_t *note, poly_synth_t *synth, int *n_voices);

#endif