#include <ncurses.h>
#include <math.h>
#include <unistd.h>
#include <alsa/asoundlib.h>

#include "defs.h"
#include "synth.h"
#include "input.h"

int main(int argc, char **argv) {

    int input = 0;

    int octave = DEFAULT_OCTAVE;
    note_t note = {.n_semitone = nC, .n_octave = octave, .n_duration = 5};
    
    int osc_a_wave = 0;
    int osc_b_wave = 0;

    double attack = 0.05;
    double decay = 0.2;
    double sustain = 0.7;
    double release = 0.2;

    if (argc >= 7) {
        attack = atof(argv[1]);
        decay = atof(argv[2]);
        sustain = atof(argv[3]);
        release = atof(argv[4]);
        osc_a_wave = atol(argv[5]);
        osc_b_wave = atol(argv[6]);
    }

    adsr_t adsr = {
        .att = attack,
        .dec = decay,
        .sus = sustain,
        .rel = release
    };

    sound_t sound = {
        .s_adsr = adsr,
        .s_active = 0,
        .s_phase = 0.0,
        .s_frames_left = 0,
        .s_frames_total = 0,
        .s_wave = osc_a_wave
    };

    sound_t sound_b = {
        .s_adsr = adsr,
        .s_active = 0,
        .s_phase = 0.0,
        .s_frames_left = 0,
        .s_frames_total = 0,
        .s_wave = osc_b_wave
    };

    synth_2osc_t synth_2osc = {
        .osc_a = &sound,
        .osc_b = &sound_b
    };

    snd_pcm_t *handle;
        
    if (snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0) {
        perror("snd_pcm_open");
        return 1;
    }

    int params_err = snd_pcm_set_params(handle, 
        SND_PCM_FORMAT_S16_LE,
        SND_PCM_ACCESS_RW_INTERLEAVED,
        1, RATE, 1, 1000000);

    if (params_err < 0) {
        fprintf(stderr, "snd_pcm_set_params error: %s\n", snd_strerror(params_err));
        return 1;
    }

    snd_pcm_prepare(handle);

    short buffer[FRAMES];
    short buffer_b[FRAMES];
    short mix_buffer[FRAMES];

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < FRAMES; j++) {
            buffer[j] = 0;
        }
        snd_pcm_writei(handle, buffer, FRAMES);
    }

    initscr();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    static int ui_counter = 0;
    
    while (input != 'x') {

        if (++ui_counter % 20 == 0) {
            clear();
            mvprintw(0, 0, "Semitone: %d", note.n_semitone);
            mvprintw(1, 0, "Octave: %d", note.n_octave);
            mvprintw(2, 0, "Frequency: %f", sound.s_freq);
            refresh();
        }

        render_synth(&synth_2osc, buffer, buffer_b, mix_buffer);
        
        int err = snd_pcm_writei(handle, mix_buffer, FRAMES);
        if (err == -EPIPE) {
            snd_pcm_prepare(handle);
        } else if (err < 0) {
            snd_pcm_prepare(handle);
        }

        input = getch();
        int note_changed = get_input(input, &note);

        if (note_changed) {
            note_to_sound(note, &sound);
            note_to_sound(note, &sound_b);
        }
    }

    snd_pcm_drain(handle);
    snd_pcm_close(handle);

    endwin();

    return 0;
}