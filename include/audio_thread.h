#ifndef __AUDIO_THREAD_H__
#define __AUDIO_THREAD_H__

#ifdef __WINDOWS__

#define __NO_RL__
#include "defs.h"
#undef __NO_RL__

#include <windows.h>

#include "synth.h"
#include "midi.h"

typedef struct audio_thread_ctx_s
{
    synth_t synth;

    /* midi */
    int midi_valid;
    midi_queue_t midi_queue;

    /* buffers */
    short display_buffer[FRAMES];
    float buffers[NUM_BUFFERS][FRAMES];
    WAVEHDR headers[NUM_BUFFERS];
    volatile LONG buffer_free[NUM_BUFFERS];
    int current_buffer;
    HWAVEOUT wave_out;

    /* effects */
    int distortion_on;
    float distortion_amount;
    float overdrive;

    /* state */
    int active_voices;
    volatile LONG should_stop;

    /* sync with main thread */
    CRITICAL_SECTION lock;
} audio_thread_ctx_t;

DWORD WINAPI audio_thread_proc(LPVOID param);

#endif

#endif