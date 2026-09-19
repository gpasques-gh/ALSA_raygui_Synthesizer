
#include <stdint.h>

#include "defs.h"
#include "synth.h"
#include "midi.h"

static void __apply_midi_event(
    synth_t *synth, 
    uint8_t status,
    uint8_t data1,
    uint8_t data2)
{
    if ((status & PRESSED) == NOTE_ON && data2 > 0)
    {
        int pressed_voices = 0;

        for (int v = 0; v < VOICES; v++)
        {   
            if (synth->voices[v].pressed)
                pressed_voices++;
            if (synth->voices[v].adsr->state == ENV_RELEASE && !synth->arp)
                synth->voices[v].adsr->state = ENV_IDLE;
        }

        voice_t *free_voice = get_free_voice(synth);
        if (free_voice == NULL) return;

        free_voice->pressed = 1;
        change_freq(free_voice, data1, data2, synth->detune);
        if (pressed_voices == 0 && synth->filter->env)
            synth->filter->adsr->state = ENV_ATTACK;
            

        if (synth->arp)
        {
            sort_synth_voices(synth);
            if (pressed_voices == 0)
            {
                synth->active_arp_float = 1.0;
            }
        }
    }
    else if ((status & PRESSED) == NOTE_OFF ||
                ((status & PRESSED) == NOTE_ON && data2 == 0))
    {
        int pressed_voices = 0;
        for (int v = 0; v < VOICES; v++)
            if (synth->voices[v].pressed)
                pressed_voices++;
        
        for (int v = 0; v < VOICES; v++)
        {
            if (synth->voices[v].note == data1 && 
                synth->voices[v].pressed)
            {
                if (synth->arp && synth->voices[v].adsr->state != ENV_IDLE)
                {
                    synth->voices[v].adsr->state = ENV_IDLE;
                }
                else if (!synth->arp &&
                        synth->voices[v].adsr->state != ENV_RELEASE &&
                        synth->voices[v].adsr->state != ENV_IDLE)
                {
                    synth->voices[v].adsr->state = ENV_RELEASE;
                }
                    
                synth->voices[v].note = -1;
                synth->voices[v].pressed = 0;

                break; 
            }
        }

        if (synth->arp)
        {
            sort_synth_voices(synth);
            if (pressed_voices == 2)
            {
                synth->active_arp_float = 1.0;
            }
        }
    }
}

#ifdef __WINDOWS__

#include "win_defs.h"
#include <windows.h>
#include <mmeapi.h>

static midi_queue_t g_midi_queue;

static volatile float latency_check;

void midi_queue_init(midi_queue_t *q)
{
    q->head = 0;
    q->tail = 0;
}

void CALLBACK MidiInProc(
    HMIDIIN midi_in, 
    UINT msg, 
    DWORD_PTR instance, 
    DWORD_PTR param1, 
    DWORD_PTR param2)
{
    if (msg != MIM_DATA)
        return;
    
    midi_queue_t *q = (midi_queue_t *)instance;
    
    BYTE status = param1 & 0xFF;
    BYTE data1 = (param1 >> 8) & 0xFF;
    BYTE data2 = (param1 >> 16) & 0xFF;

    LONG head = q->head;
    LONG next = (head + 1) & (MIDI_QUEUE_SIZE - 1);

    if (next == q->tail)
        return;

    q->events[head].status = status;
    q->events[head].data1 = data1;
    q->events[head].data2 = data2;

    MemoryBarrier();
    InterlockedExchange(&q->head, next);
}

void poll_midi_queue(midi_queue_t *q, synth_t *synth)
{
    LONG tail = q->tail;
    while (tail != q->head)
    {
        midi_event_t ev = q->events[tail];
        __apply_midi_event(synth, ev.status, ev.data1, ev.data2);
        tail = (tail + 1) & (MIDI_QUEUE_SIZE - 1);
    }
    InterlockedExchange(&q->tail, tail);
}
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
             float *attack, float *decay, float *sustain, float *release)
{   
    unsigned char midi_buffer[1024];
    ssize_t ret = snd_rawmidi_read(midi_in, midi_buffer, sizeof(midi_buffer));

    if (ret < 0)
    {
        return 1;
    }
        
    for (int i = 0; i + 2 < ret; i += 3)
    {   /* Getting the MIDI bytes informations */
        unsigned char status = midi_buffer[i];
        unsigned char data1 = midi_buffer[i + 1];
        unsigned char data2 = midi_buffer[i + 2];

        __apply_midi_event(synth, status, data1, data2);
    }

    return 0;
}

#endif 