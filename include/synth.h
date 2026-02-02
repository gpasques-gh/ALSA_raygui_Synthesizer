#ifndef SYNTH_H
#define SYNTH_H

#define SINE_WAVE 0
#define SQUARE_WAVE 1
#define TRIANGLE_WAVE 2
#define SAWTOOTH_WAVE 3

typedef struct {
    short semitone;
    short octave;
    short duration;
    short velocity;
} note_t;

typedef struct {
    double att;
    double dec;
    double sus;
    double rel;
} adsr_t;

typedef struct {
    double freq;
    double phase;
    int frames_left;
    int frames_total;
    int active;
    short wave;
} osc_t;

typedef struct {
    float alpha; 
    float prev;
} lp_filter_t;

typedef struct {
    osc_t *osc_a;
    osc_t *osc_b;
    osc_t *osc_c;
    adsr_t *adsr;
    lp_filter_t *lp_filter;
    int frames_left;
    int frames_total;
    int active;
    double detune;
    double velocity_amplitude;
} synth_3osc_t;

// SOUND RELATED
double get_adsr_envelope(synth_3osc_t *synth);
void change_osc_freq(synth_3osc_t *synth, note_t note);
char *get_wave_name(int wave);

// SOUND WAVES
void render_osc(osc_t *osc,  short *buffer);
void render_sine(osc_t *osc, short *buffer);
void render_square(osc_t *osc, short *buffer);
void render_triangle(osc_t *osc, short *buffer);
void render_sawtooth(osc_t *osc, short *buffer);
void render_synth3osc(synth_3osc_t *synth, short *mix_buffer);

// FILTER
void lp_init(lp_filter_t *filter, float cutoff);
short lp_process(lp_filter_t *filter, short input);

#endif