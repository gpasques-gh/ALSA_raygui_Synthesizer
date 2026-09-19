#include "audio_thread.h"
#include "synth.h"
#include "effects.h"

#ifdef __WINDOWS__

/* Callback function for the sound card */
void CALLBACK waveOutProc(
    HWAVEOUT wave_out,
    UINT msg,
    DWORD_PTR instance,
    DWORD_PTR param1,
    DWORD_PTR param2)
{
    if (msg == WOM_DONE)
    {
        audio_thread_ctx_t *ctx = (audio_thread_ctx_t *)instance;
        WAVEHDR *hdr = (WAVEHDR *)param1;
        int idx = (int)((float *)hdr->lpData - &ctx->buffers[0][0]) / FRAMES;
        InterlockedExchange(&ctx->buffer_free[idx], 1);
    }
}

/* Audio thread function */
DWORD WINAPI audio_thread_proc(LPVOID param)
{
    audio_thread_ctx_t *ctx = (audio_thread_ctx_t *)param;

    timeBeginPeriod(1);

    /* Getting the state of the program from the main thread */
    while (!InterlockedCompareExchange(&ctx->should_stop, 0, 0))
    {
        /* Poll MIDI queue */
        if (ctx->midi_valid)
            poll_midi_queue(&ctx->midi_queue, &ctx->synth);

        /* Count active voices */
        for (int v = 0; v < VOICES; v++)
            if (ctx->synth.voices[v].adsr->state != ENV_IDLE)
                ctx->active_voices++;

        /* Get the current buffer index */
        int buf_idx = ctx->current_buffer;

        /* Create a local buffer before float conversion */
        short local_buf[FRAMES];

        /* Process the synthesizer data */
        for (int i = 0; i < FRAMES; i++)
        {
            process_lfo(&ctx->synth);
            double sample = process_voices(&ctx->synth);
            sample = process_gain(ctx->synth, sample, ctx->active_voices);
            sample = process_filter(&ctx->synth, sample);
            local_buf[i] = (short)(sample * 32767.0);
            if (ctx->distortion_on)
            {
                local_buf[i] = distortion(local_buf[i],
                    ctx->distortion_amount, ctx->overdrive);
            }
            process_arpeggiator(&ctx->synth, ctx->active_voices);
        }

        ctx->active_voices = 0;
        // float float_buf[FRAMES];
        // for (int i = 0; i < FRAMES; i++)
        //     float_buf[i] = (float)local_buf[i] / 32768.0f;

        /* Critical Section : copy the local buffer to the context display buffer */
        /* The display buffer is used in the main thread for the waveform visualizer */
        EnterCriticalSection(&ctx->lock);
        memcpy(ctx->display_buffer, local_buf, sizeof(local_buf));
        LeaveCriticalSection(&ctx->lock);

        /* Check if the current buffer is free */
        while (!InterlockedCompareExchange(&ctx->buffer_free[buf_idx], 0, 1))
            Sleep(1);

        /* Reset the current header */
        if (ctx->headers[buf_idx].dwFlags & WHDR_PREPARED)
            waveOutUnprepareHeader(
                ctx->wave_out,
                &ctx->headers[buf_idx],
                sizeof(WAVEHDR));

        /* Convert the integer data to float data */
        for (int i = 0; i < FRAMES; i++)
            ctx->buffers[buf_idx][i] = (float)local_buf[i] / 32768.0f;

        /* Update the headers */
        ctx->headers[buf_idx].lpData = (LPSTR)ctx->buffers[buf_idx];
        ctx->headers[buf_idx].dwBufferLength = FRAMES * sizeof(float);
        ctx->headers[buf_idx].dwFlags = 0;

        /* Write the float data to the sound card */
        waveOutPrepareHeader(ctx->wave_out, &ctx->headers[buf_idx], sizeof(WAVEHDR));
        waveOutWrite(ctx->wave_out, &ctx->headers[buf_idx], sizeof(WAVEHDR));

        /* Increment the current buffer */
        ctx->current_buffer = (buf_idx + 1) % NUM_BUFFERS;
    }

    timeEndPeriod(1);

    return 0;
}

#endif