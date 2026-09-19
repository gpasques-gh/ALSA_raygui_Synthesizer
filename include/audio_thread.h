#ifndef __AUDIO_THREAD_H__
#define __AUDIO_THREAD_H__

#ifdef __WINDOWS__

#define __NO_RL__
#include "defs.h"
#undef __NO_RL__

#include <windows.h>

#include "synth.h"
#include "midi.h"

/* Audio thread context structure */
typedef struct audio_thread_ctx_s
{
    synth_t synth;

    /* MIDI */
    int midi_valid;
    midi_queue_t midi_queue;

    /* Audio buffers */
    short display_buffer[FRAMES];
    float buffers[NUM_BUFFERS][FRAMES];
    WAVEHDR headers[NUM_BUFFERS];
    volatile LONG buffer_free[NUM_BUFFERS];
    int current_buffer;
    HWAVEOUT wave_out;

    /* Audio effects variables */
    int distortion_on;
    float distortion_amount;
    float overdrive;

    /* Synthesizer and program state */
    int active_voices;
    volatile LONG should_stop;

    /* Critical section */
    CRITICAL_SECTION lock;
} audio_thread_ctx_t;

/* Callback function for the sound card */
void CALLBACK waveOutProc(
    HWAVEOUT wave_out,
    UINT msg,
    DWORD_PTR instance,
    DWORD_PTR param1,
    DWORD_PTR param2);

/* Audio thread function */
DWORD WINAPI audio_thread_proc(LPVOID param);

#endif /* __WINDOWS__ */
#endif /* __AUDIO_THREAD_H__ */