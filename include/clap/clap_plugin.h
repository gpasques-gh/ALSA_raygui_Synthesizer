#ifndef __CLAP_PLUGIN_H__
#define __CLAP_PLUGIN_H__

#ifdef __CLAP__

#ifdef _WIN32
#include <windows.h>

typedef HANDLE mutex;
#define mutex_acquire(mutex) WaitForSingleObject(mutex, INFINITE)
#define mutex_release(mutex) ReleaseMutex(mutex)
#define mutex_init(mutex) (mutex = CreateMutex(NULL, FALSE, NULL))
#define mutex_destroy(mutex) CloseHandle(mutex)

#elif defined(__linux__)

#include <pthread.h>
#define mutex_acquire(mutex) pthread_mutex_lock(&(mutex))
#define mutex_release(mutex) pthread_mutex_unlock(&(mutex))
#define mutex_init(mutex) pthread_mutex_init(&(mutex), NULL)
#define mutex_destroy(mutex) pthread_mutex_destroy(&(mutex))

#endif

#define P_VOLUME (0)
#define P_WAVE_A (1)
#define P_WAVE_B (2)
#define P_WAVE_C (3)
#define P_DETUNE (4)
#define P_ATTACK (5)
#define P_DECAY (6)
#define P_SUSTAIN (7)
#define P_RELEASE (8)
#define P_CUTOFF (9)
#define P_COUNT (10)

#include "clap_lib/clap.h"
#include "core/synth.h"

#include <stdatomic.h>

/* Synth CLAP plugin structure */
typedef struct
{
	clap_plugin_t plugin;
	const clap_host_t *host;
	double sample_rate;
	
	/* Synthesizer*/
	synth_t synth;

	/* Parameters */
	_Atomic float params[P_COUNT];
	
} synth_plugin_t;

void process_event(
	synth_plugin_t *p,
	const clap_event_header_t *hdr);

clap_process_status plugin_process(
	const clap_plugin_t *plugin,
	const clap_process_t *process);

/* Plugin functions */
bool plugin_init(const clap_plugin_t *plugin);
void plugin_destroy(const clap_plugin_t *plugin);
bool plugin_activate(
	const clap_plugin_t *plugin, 
	double sample_rate,
	uint32_t min_frames, 
	uint32_t max_frames);
void plugin_deactivate(const clap_plugin_t *plugin);
bool plugin_start_processing(const clap_plugin_t *plugin);
void plugin_stop_processing(const clap_plugin_t *plugin);
void plugin_reset(const clap_plugin_t *plugin);
void plugin_on_main_thread(const clap_plugin_t *plugin);
const void *plugin_get_extension(
	const clap_plugin_t *plugin, const char *id);

#endif /* __CLAP__ */
#endif /* __CLAP_PLUGIN_H__ */