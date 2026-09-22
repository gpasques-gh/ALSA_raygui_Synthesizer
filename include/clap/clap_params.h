#ifdef __CLAP__

#ifndef __CLAP_PARAMS_H__
#define __CLAP_PARAMS_H__

#include "clap/clap_plugin.h"

/* Parameters defines */

/* Volume */
#define PARAM_VOLUME { P_VOLUME, "Volume", "Master", CLAP_PARAM_IS_AUTOMATABLE | CLAP_PARAM_REQUIRES_PROCESS, 0.0, 1.0, 1.0 }

/* Waveforms */
#define PARAM_WAVE_TEMPLATE CLAP_PARAM_IS_STEPPED | CLAP_PARAM_IS_ENUM | CLAP_PARAM_IS_AUTOMATABLE | CLAP_PARAM_REQUIRES_PROCESS, SINE_WAVE, SAWTOOTH_WAVE, SINE_WAVE
#define PARAM_WAVE_A { P_WAVE_A, "Wave", "Oscillator A", PARAM_WAVE_TEMPLATE }
#define PARAM_WAVE_B { P_WAVE_B, "Wave", "Oscillator B", PARAM_WAVE_TEMPLATE }
#define PARAM_WAVE_C { P_WAVE_C, "Wave", "Oscillator C", PARAM_WAVE_TEMPLATE }

/* Detune */
#define PARAM_DETUNE { P_DETUNE, "Detune", "Oscillators", CLAP_PARAM_IS_AUTOMATABLE | CLAP_PARAM_REQUIRES_PROCESS, 0.0, 5.0, 0.0 }

/* ADSR Envelope */
#define PARAM_ATTACK { P_ATTACK, "Attack", "Amplitude Envelope", CLAP_PARAM_IS_AUTOMATABLE | CLAP_PARAM_REQUIRES_PROCESS, 0.0, 2.0, 0.2 }
#define PARAM_DECAY { P_DECAY, "Decay", "Amplitude Envelope", CLAP_PARAM_IS_AUTOMATABLE | CLAP_PARAM_REQUIRES_PROCESS, 0.0, 2.0, 0.3 }
#define PARAM_SUSTAIN { P_SUSTAIN, "Sustain", "Amplitude Envelope", CLAP_PARAM_IS_AUTOMATABLE | CLAP_PARAM_REQUIRES_PROCESS, 0.0, 1.0, 0.7 }
#define PARAM_RELEASE { P_RELEASE, "Release", "Amplitude Envelope", CLAP_PARAM_IS_AUTOMATABLE | CLAP_PARAM_REQUIRES_PROCESS, 0.0, 2.0, 0.2 }

/* Low-Pass Filter */
#define PARAM_CUTOFF { P_CUTOFF, "Cutoff", "Low-Pass Filter", CLAP_PARAM_IS_AUTOMATABLE | CLAP_PARAM_REQUIRES_PROCESS, 0.0, 1.0, 0.5 }


typedef struct 
{
	clap_id id;
	const char *name;
	const char *module;
	clap_param_info_flags flags;
	double min, max, def;
} param_desc_t;

extern const param_desc_t PARAMS[P_COUNT];
extern const clap_plugin_params_t params_ext;

const param_desc_t *param_desc_from_id(clap_id id);

#endif 
#endif