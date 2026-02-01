#ifndef MIDI_H
#define MIDI_H

#include <alsa/asoundlib.h>
#include "synth.h"

#define MIDI_MAX_VALUE 127.0

#define PRESSED 0xF0
#define NOTE_PRESSED 0x90
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

// EFFECTS
#define ARTURIA_CUTOFF_KNOB 74
#define ARTURIA_DETUNE_KNOB 71

int get_midi(snd_rawmidi_t *midi_in, note_t *note, synth_3osc_t *synth);

#endif