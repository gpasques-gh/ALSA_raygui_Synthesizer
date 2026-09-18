#define RAYGUI_IMPLEMENTATION

#ifdef __WINDOWS__

#include "win_defs.h"
#include "raylib/src/raygui.h"
#include <windows.h>

#ifdef NOGDI
typedef struct tagBITMAPINFOHEADER {
    DWORD biSize;
    LONG  biWidth;
    LONG  biHeight;
    WORD  biPlanes;
    WORD  biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG  biXPelsPerMeter;
    LONG  biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER, *LPBITMAPINFOHEADER;
#endif

#include <mmsystem.h>
#include <mmreg.h>
#include <ks.h>
#include <ksmedia.h>

#elif defined(__LINUX__)

#include <unistd.h>
#include <alsa/asoundlib.h>
#include "xml.h"
#include <raygui.h>

#endif

#include <float.h>
#include <stdint.h>

#include "defs.h"
#include "interface.h"
#include "synth.h"
#include "keyboard.h"
#include "record.h"
#include "effects.h"
#include "midi.h"

/* Prints the usage of the CLI arguments into the error output */
void usage()
{
    fprintf(stderr, "synth -midi <midi hardware id> : midi keyboard input, able to change parameters of the sounds (ADSR, cutoff, detune and oscillators waveforms)\n");
    fprintf(stderr, "use amidi -l to list your connected midi devices and find your midi device hardware id, often something like : hw:0,0,0 or hw:1,0,0\n");
    fprintf(stderr, "to see this helper again, use synth -h or synth -help\n");
}

#if !defined(__WINDOWS__) && !defined(__LINUX__)
    #error "Unsupported OS."
#endif

#ifdef __WINDOWS__
#define NUM_BUFFERS 4

static HWAVEOUT wave_out;
static WAVEHDR headers[NUM_BUFFERS];
static volatile LONG done = 0;
static float buffers[NUM_BUFFERS][FRAMES];
static volatile LONG buffer_free[NUM_BUFFERS] = {1, 1, 1, 1};

void CALLBACK waveOutProc(
    HWAVEOUT wave_out,
    UINT msg,
    DWORD_PTR instance,
    DWORD_PTR param1,
    DWORD_PTR param2)
{
    if (msg == WOM_DONE)
    {
        WAVEHDR *hdr = (WAVEHDR *)param1;
        int32_t index = (int32_t)(hdr - headers);
        InterlockedExchange(&buffer_free[index], 1);
    }
}
#endif 

/* Main function */
int main(int argc, char **argv)
{
    char midi_device[256];
    int midi_input = 0;

    if (argc > 1)
    {
        if (strcmp(argv[1], "-midi") == 0 && argc >= 3)
        {
            midi_input = 1;
            strncpy(midi_device, argv[2], sizeof(midi_device) - 1);
            midi_device[sizeof(midi_device) - 1] = '\0';
        }
        else if (strcmp(argv[1], "-midi") == 0 && argc < 3)
        {
            fprintf(stderr, "missing midi hardware device id. \n");
            return 1;
        }
        else
        {
            usage();
            return 1;
        }
    }
    
    int octave = DEFAULT_OCTAVE;

    int wave_a = SINE_WAVE, wave_b = SINE_WAVE, wave_c = SINE_WAVE;
    int osc_lfo = SINE_WAVE;

    float attack = 0.2;
    float decay = 0.3;
    float sustain = 0.7;
    float release = 0.2;

    float filter_attack = 0.0;
    float filter_decay = 0.3;
    float filter_sustain = 0.0;
    float filter_release = 0.2;

    adsr_t filter_adsr =
        {
            .attack = &filter_attack,
            .decay = &filter_decay,
            .sustain = &filter_sustain,
            .release = &filter_release};

    lp_filter_t filter =
        {
            .cutoff = 0.5,
            .prev_input = 0.0,
            .prev_output = 0.0,
            .adsr = &filter_adsr,
            .env = false};

    osc_t lfo_osc =
        {
            .freq = 0.5,
            .phase = 0.0,
            .wave = &osc_lfo};

    lfo_t lfo = 
        {
            .osc = &lfo_osc,
            .mod_param = LFO_OFF};
    
    synth_t synth =
        {
            .voices = malloc(sizeof(voice_t) * VOICES),
            .amp = DEFAULT_AMPLITUDE,
            .detune = 0.0,
            .filter = &filter,
            .lfo = &lfo,
            .arp = false,
            .active_arp = 0,
            .active_arp_float = 1.0,
            .bpm = 150.0};

    if (synth.voices == NULL)
    {
        fprintf(stderr, "memory allocation failed.\n");
        return 1;
    }

    /* Create the synth voices */
    for (int i = 0; i < VOICES; i++)
    {
        synth.voices[i].adsr = malloc(sizeof(adsr_t));
        if (synth.voices[i].adsr == NULL)
        {
            fprintf(stderr, "memory allocation failed.\n");
            for (int j = 0; j < i; j++)
            {
                free(synth.voices[j].adsr);
                free(synth.voices[j].oscillators);
            }
            free(synth.voices);
            return 1;
        }

        synth.voices[i].adsr->attack = &attack;
        synth.voices[i].adsr->decay = &decay;
        synth.voices[i].adsr->sustain = &sustain;
        synth.voices[i].adsr->release = &release;
        synth.voices[i].adsr->state = ENV_IDLE;
        synth.voices[i].adsr->output = 0.0;

        synth.voices[i].note = -1;
        synth.voices[i].velocity_amp = 0.0;
        synth.voices[i].pressed = 0;

        synth.voices[i].oscillators = malloc(sizeof(osc_t) * 3);
        if (synth.voices[i].oscillators == NULL)
        {
            fprintf(stderr, "memory allocation failed.\n");
            goto cleanup_synth;
        }

        for (int j = 0; j < 3; j++)
        {
            synth.voices[i].oscillators[j].freq = 0.0;
            synth.voices[i].oscillators[j].phase = 0.0;
        }
        synth.voices[i].oscillators[0].wave = &wave_a;
        synth.voices[i].oscillators[1].wave = &wave_b;
        synth.voices[i].oscillators[2].wave = &wave_c;
    }

    short buffer[FRAMES];
    memset(buffer, 0, sizeof(short) * FRAMES);
#ifdef __WINDOWS__

    UINT sound_devices = waveOutGetNumDevs();
    for (UINT i = 0; i < sound_devices; i++)
    {
        WAVEOUTCAPS caps;
        if (waveOutGetDevCaps(i, &caps, sizeof(WAVEOUTCAPS)) == MMSYSERR_NOERROR)
            fprintf(stderr, "device %u: %s\n", i, caps.szPname);
    }

    timeBeginPeriod(1);

    WAVEFORMATEXTENSIBLE wfx = {0};
    wfx.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
    wfx.Format.nChannels = MONO;
    wfx.Format.nSamplesPerSec = RATE;
    wfx.Format.wBitsPerSample = 32;
    wfx.Format.nBlockAlign = wfx.Format.nChannels * (wfx.Format.wBitsPerSample / 8);
    wfx.Format.nAvgBytesPerSec = wfx.Format.nSamplesPerSec * wfx.Format.nBlockAlign;
    wfx.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);

    wfx.Samples.wValidBitsPerSample = 32;
    wfx.dwChannelMask = SPEAKER_FRONT_CENTER;
    wfx.SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;

    MMRESULT sound_res = waveOutOpen(
        &wave_out,
        WAVE_MAPPER,
        (WAVEFORMATEX *)&wfx,
        (DWORD_PTR)waveOutProc,
        0, CALLBACK_FUNCTION);

    if (sound_res != MMSYSERR_NOERROR)
        return 1;

    uint8_t current_buffer = 0;

    HMIDIIN midi_in;
    midi_queue_t midi_queue;

    uint8_t midi_valid = 1;
    LONG midi_id;
    UINT midi_devices;

    if (midi_input)
    {
        midi_devices = midiInGetNumDevs();
        if (midi_devices == 0)
        {
            fprintf(stderr, "no midi device found.\n");
            midi_valid = 0;
        }

        char *endptr;
        midi_id = 
            strtol(midi_device, &endptr, 10);
        if (endptr == midi_device)
            midi_valid = 0;
        else if (midi_id < 0 || midi_id > midi_devices)
            midi_valid = 0;
    }

    if (midi_input && midi_valid)
    {
        for (UINT i = 0; i < midi_devices; i++)
        {
            MIDIINCAPS caps;
            midiInGetDevCaps(i, &caps, sizeof(MIDIINCAPS));
            printf("midi_device: [%u] %s\n", i, caps.szPname);
        }

        midi_queue_init(&midi_queue);

        MMRESULT midi_res = midiInOpen(
            &midi_in, 1, 
            (DWORD_PTR)MidiInProc, 
            (DWORD_PTR)&midi_queue, 
            CALLBACK_FUNCTION);

        if (midi_res != MMSYSERR_NOERROR)
        {
            fprintf(stderr, "error while opening midi device.\n");
            return 1;
        }

        midiInStart(midi_in);
    }

#elif defined(__LINUX__)
    snd_pcm_t *handle = NULL;
    if (snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0)
    {
        fprintf(stderr, "error while opening sound card.\n");
        goto cleanup_synth;
    }

    int params_err = snd_pcm_set_params(
        handle,
        SND_PCM_FORMAT_S16_LE,
        SND_PCM_ACCESS_RW_INTERLEAVED,
        MONO, RATE, 1, LATENCY);

    if (params_err < 0)
    {
        fprintf(stderr, "error while setting sound card parameters: %s\n", snd_strerror(params_err));
        goto cleanup_alsa;
    }

    snd_pcm_prepare(handle);
    snd_pcm_writei(handle, buffer, FRAMES);

    snd_rawmidi_t *midi_in;
    if (midi_input)
    {
        if (snd_rawmidi_open(&midi_in, NULL, midi_device, SND_RAWMIDI_NONBLOCK) < 0)
        {
            fprintf(stderr, "error while opening midi device %s\n", midi_device);
            goto cleanup_alsa;
        }
    }
#endif
    char audio_filename[1024] = "\0";
    FILE *fwav = NULL;
    wav_header_t header;
    unsigned int count = 0;
    bool recording = false;

    /* Oscillators dropdown menus booleans */
    bool ddm_a = false, ddm_b = false, ddm_c = false;
    bool saving_preset = false, saving_audio_file = false, loading_preset = false;
    bool lfo_wave_ddm = false, lfo_params_ddm = false;
    bool distortion_on = false, overdrive = false;
    float distortion_amount = 0.0;
    int active_voices = 0;

    char preset_filename[1024] = "\0";

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(WIDTH, HEIGHT, "ALSA & raygui synthesizer");
    Font annotation = LoadFont("Regular.ttf");
    GuiSetFont(annotation);
    GuiSetStyle(DEFAULT, TEXT_SIZE, GuiGetFont().baseSize * 0.5);

    while (!WindowShouldClose())
    {
        if (!saving_preset && !saving_audio_file)
        {
            handle_input(&synth, &octave, 
				&attack, &decay, &sustain, &release,
				&wave_a, &wave_b, &wave_c);
            handle_release(&synth, octave);
        }

#ifdef __WINDOWS__
        if (midi_valid)
            poll_midi_queue(&midi_queue, &synth);
#elifdef __LINUX__
        if (midi_input)
            get_midi(midi_in, &synth, &attack, &decay, &sustain, &release);
#endif
        for (int v = 0; v < VOICES; v++)
        {
            if (synth.voices[v].adsr->state != ENV_IDLE)
            {
                active_voices++;
            }
        }
            
        for (int i = 0; i < FRAMES; i++)
        {
            process_lfo(&synth);
            double sample = process_voices(&synth);
            sample = process_gain(synth, sample, active_voices);
            sample = process_filter(&synth, sample);
            buffer[i] = (short)(sample * 32767.0);
            if (distortion_on)
            {
                buffer[i] = distortion(buffer[i], distortion_amount, overdrive);
            }
            process_arpeggiator(&synth, active_voices);
        }

        active_voices = 0;
#ifdef __WINDOWS__
        while (!InterlockedCompareExchange(&buffer_free[current_buffer], 0, 1))
            Sleep(1);

        if (headers[current_buffer].dwFlags & WHDR_PREPARED)
            waveOutUnprepareHeader(wave_out, &headers[current_buffer], sizeof(WAVEHDR));

        for (uint16_t i = 0; i < FRAMES; i++)
            buffers[current_buffer][i] =
                (float)buffer[i] / 32768.0f;

        headers[current_buffer].lpData = (LPSTR)buffers[current_buffer];
        headers[current_buffer].dwBufferLength = FRAMES * sizeof(float);
        headers[current_buffer].dwFlags = 0;

        waveOutPrepareHeader(wave_out, &headers[current_buffer], sizeof(WAVEHDR));
        waveOutWrite(wave_out, &headers[current_buffer], sizeof(WAVEHDR));
        
        current_buffer = (current_buffer + 1) % NUM_BUFFERS;
#elif defined(__LINUX__)
        int err = snd_pcm_writei(handle, buffer, FRAMES);
        if (err == -EPIPE)
        {
            fprintf(stderr, "ALSA underrun!\n");
            snd_pcm_prepare(handle);
        }
        else if (err < 0)
        {
            fprintf(stderr, "ALSA write error: %s\n", snd_strerror(err));
            snd_pcm_prepare(handle);
        }
#endif 
        if (fwav != NULL && recording == true)
        {
            fwrite(buffer, 2, FRAMES, fwav);
            count++;
        }
        else if (fwav == NULL && recording == true)
        {
            char audio_full_filename[1024] = "audio/";
            strcat(audio_full_filename, audio_filename);
            strcat(audio_full_filename, ".wav");
            init_wav_header(&header);
            init_wav_file(audio_full_filename, &fwav, &header);
            audio_filename[0] = '\0';
        }
        else if (fwav != NULL && recording == false)
        {
            header.sub2_size = FRAMES * count * (unsigned int)header.num_channels * (unsigned int)header.bits_per_sample / 8;
            header.chunk_size = (unsigned int)header.sub2_size + 36;
            fseek(fwav, 0, SEEK_SET);
            fwrite(&header, 1, sizeof(header), fwav);
            close_wav_file(fwav);
            fwav = NULL;
            count = 0;
        }

        BeginDrawing();

            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
            GuiLabel((Rectangle){WIDTH / 2 - 115, 5, 230, 20}, "ALSA & raygui Synthesizer");
            
            render_waveform(buffer);
            render_adsr(&attack, &decay, &sustain, &release);
            render_filter_adsr(&synth);
            render_osc_waveforms(
                &wave_a, &wave_b, &wave_c,
                &ddm_a, &ddm_b, &ddm_c);
            render_synth_params(&synth);
            render_options(
                &synth,
                audio_filename,
                &saving_preset, &loading_preset, 
                &saving_audio_file, &recording);
            render_effects(
                &synth,
                &lfo_wave_ddm, &lfo_params_ddm,
                &distortion_on, &overdrive,
                &distortion_amount);
#ifdef __LINUX__
            if (loading_preset)
            {
                load_preset(
                    &synth,
                    &attack, &decay, &sustain, &release,
                    &wave_a, &wave_b, &wave_c,
                    &distortion_on, &overdrive, 
                    &distortion_amount, 
                    &loading_preset);
            }
                
            if (saving_preset)
            {
                save_preset(
                    synth,
                    attack, decay, sustain, release,
                    wave_a, wave_b, wave_c,
                    preset_filename, &saving_preset, 
                    distortion_on, overdrive,
                    distortion_amount);
            }
#endif
            render_white_keys();
            for (int v = 0; v < VOICES; v++)
            {
                if (synth.voices[v].pressed && !is_black_key(synth.voices[v].note))
                {
                    render_key(synth.voices[v].note, false);
                }
            }
            if (synth.arp && 
                !is_black_key(synth.voices[synth.active_arp].note) &&
                synth.voices[synth.active_arp].pressed)
            {
                render_key(synth.voices[synth.active_arp].note, true);
            }

            render_black_keys();
            for (int v = 0; v < VOICES; v++)
            {
                if (synth.voices[v].pressed && is_black_key(synth.voices[v].note))
                {
                    render_key(synth.voices[v].note, false);
                }
            }
            if (synth.arp && 
                is_black_key(synth.voices[synth.active_arp].note) &&
                synth.voices[synth.active_arp].pressed)
            {
                render_key(synth.voices[synth.active_arp].note, true);
            }
                
    
        EndDrawing();
    }

    CloseWindow();

#ifdef __LINUX__
    if (midi_in)
    {
        snd_rawmidi_close(midi_in);
    }
#elif defined(__WINDOWS__)
    waveOutClose(wave_out);
    timeEndPeriod(1);

    if (midi_valid)
    {
        midiInStop(midi_in);
        midiInClose(midi_in);
    }
#endif
    /* If we quit the application during recording, change WAV header and close WAV file */
    if (fwav != NULL && recording)
    {
        header.sub2_size = FRAMES * count * (unsigned int)header.num_channels * (unsigned int)header.bits_per_sample / 8;
        header.chunk_size = (unsigned int)header.sub2_size + 36;
        fseek(fwav, 0, SEEK_SET);
        fwrite(&header, 1, sizeof(header), fwav);
        close_wav_file(fwav);
    }

#ifdef __LINUX__
cleanup_alsa:
    if (handle)
    {
        snd_pcm_drain(handle);
        snd_pcm_close(handle);
    }
#endif
cleanup_synth:
    for (int i = 0; i < VOICES; i++)
    {
        free(synth.voices[i].adsr);
        free(synth.voices[i].oscillators);
    }
    free(synth.voices);


    return 0;
}
