#ifndef MIDI_H
#define MIDI_H

#include "synth.h"
#include <stdint.h>

#ifdef __WINDOWS__

#include "win_defs.h"
#include <windows.h>
#include <mmeapi.h>

#define MIDI_QUEUE_SIZE 256

typedef struct
{
    uint8_t status;
    uint8_t data1;
    uint8_t data2;
} midi_event_t;

typedef struct 
{
    midi_event_t events[MIDI_QUEUE_SIZE];
    volatile LONG head;
    volatile LONG tail;
} midi_queue_t;

void midi_queue_init(midi_queue_t *q);
void poll_midi_queue(midi_queue_t *q, synth_t *synth);

void CALLBACK MidiInProc(
    HMIDIIN midi_in, 
    UINT msg, 
    DWORD_PTR instance, 
    DWORD_PTR param1, 
    DWORD_PTR param2);
#elif defined(__LINUX__)
#include <alsa/asoundlib.h>
/*
 * Get the MIDI input from the ALSA RawMIDI input (snd_rawmidi_t)
 * Activate the synth voices and update their frequencies with the given note
 * Turn off the synth voices when their assigned note are being released
 * Change the ADSR parameters when the assigned knobs are being triggered
 * Change the cutoff, detune and amplification when the assigned knobs are being triggered
 */
int get_midi(snd_rawmidi_t *midi_in, synth_t *synth,
             float *attack, float *decay, float *sustain, float *release);

#endif /* OS */
#endif /* MIDI_H */