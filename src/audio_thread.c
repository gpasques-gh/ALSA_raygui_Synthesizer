#include "audio_thread.h"
#include "synth.h"
#include "effects.h"

#ifdef __WINDOWS__

DWORD WINAPI audio_thread_proc(LPVOID param)
{
    audio_thread_ctx_t *ctx = (audio_thread_ctx_t *)param;

    timeBeginPeriod(1);

    while (!InterlockedCompareExchange(&ctx->should_stop, 0, 0))
    {
        if (ctx->midi_valid)
            poll_midi_queue(&ctx->midi_queue, &ctx->synth);

        for (int v = 0; v < VOICES; v++)
            if (ctx->synth.voices[v].adsr->state != ENV_IDLE)
                ctx->active_voices++;

        int buf_idx = ctx->current_buffer;

        short local_buf[FRAMES];
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

        float float_buf[FRAMES];
        for (int i = 0; i < FRAMES; i++)
            float_buf[i] = (float)local_buf[i] / 32768.0f;

        EnterCriticalSection(&ctx->lock);
        memcpy(ctx->display_buffer, local_buf, sizeof(local_buf));
        LeaveCriticalSection(&ctx->lock);

        while (!InterlockedCompareExchange(&ctx->buffer_free[buf_idx], 0, 1))
            Sleep(1);

        if (ctx->headers[buf_idx].dwFlags & WHDR_PREPARED)
            waveOutUnprepareHeader(
                ctx->wave_out,
                &ctx->headers[buf_idx],
                sizeof(WAVEHDR));

        memcpy(ctx->buffers[buf_idx], float_buf, sizeof(float_buf));

        ctx->headers[buf_idx].lpData = (LPSTR)ctx->buffers[buf_idx];
        ctx->headers[buf_idx].dwBufferLength = FRAMES * sizeof(float);
        ctx->headers[buf_idx].dwFlags = 0;

        waveOutPrepareHeader(ctx->wave_out, &ctx->headers[buf_idx], sizeof(WAVEHDR));
        waveOutWrite(ctx->wave_out, &ctx->headers[buf_idx], sizeof(WAVEHDR));

        ctx->current_buffer = (buf_idx + 1) % NUM_BUFFERS;
    }

    timeEndPeriod(1);

    return 0;
}

#endif