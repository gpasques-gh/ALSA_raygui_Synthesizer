#include <alsa/asoundlib.h>

#include "synth.h"
#include "defs.h"
#include "midi.h"


/**
 * Function that read midi input from the snd_rawmidi_t input
 * Into a note_t struct that is then used to change the frequency of the synth_3osc_t oscillators
 */
int get_midi(snd_rawmidi_t *midi_in, note_t *note, synth_3osc_t *synth) {

    unsigned char midi_buffer[1024];
    ssize_t ret = snd_rawmidi_read(midi_in, midi_buffer, sizeof(midi_buffer));
    
    if (ret < 0) {
        return 1;
    }

    for (int i = 0; i + 2 < ret; i += 3) {
        unsigned char status = midi_buffer[i];
        unsigned char data1 = midi_buffer[i+1]; // Note if key pressed, CC value if knob turned
        unsigned char data2 = midi_buffer[i+2]; // Velocity if key pressed, Knob turn value if knob turned

        if ((status & PRESSED) == NOTE_PRESSED && data2 > 0) {
            note->semitone = (data1 % 12);
            note->octave = (data1 / 12) - 1;
            note->velocity = data2;
            change_osc_freq(synth, *note);
        }

        if ((status & PRESSED) == KNOB_TURNED) {
            switch(data1) {
                case ARTURIA_ATT_KNOB:
                    synth->adsr->att = (data2 / MIDI_MAX_VALUE);
                    break;
                case ARTURIA_DEC_KNOB:
                    synth->adsr->dec = (data2 / MIDI_MAX_VALUE);
                    break;
                case ARTURIA_SUS_KNOB:
                    synth->adsr->sus = (data2 / MIDI_MAX_VALUE);
                    break;
                case ARTURIA_REL_KNOB:
                    synth->adsr->rel = (data2 / MIDI_MAX_VALUE);
                    break;
                case ARTURIA_OSC_A_KNOB:
                    synth->osc_a->wave = (int)((data2 * 4) / (MIDI_MAX_VALUE + 1));
                    break;
                case ARTURIA_OSC_B_KNOB:
                    synth->osc_b->wave = (int)((data2 * 4) / (MIDI_MAX_VALUE + 1));
                    break;
                case ARTURIA_OSC_C_KNOB:
                    synth->osc_c->wave = (int)((data2 * 4) / (MIDI_MAX_VALUE + 1));
                    break;
                case ARTURIA_CUTOFF_KNOB:
                    double cutoff = (data2 / MIDI_MAX_VALUE) * (RATE / 2);
                    lp_init(synth->lp_filter, cutoff);
                    break;
                case ARTURIA_DETUNE_KNOB:
                    synth->detune = (data2 / MIDI_MAX_VALUE);
                    break;
                default:
                    break;
            }
        }
    }

    return 0;
    
}