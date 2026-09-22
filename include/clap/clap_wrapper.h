#ifndef __CLAP_WRAPPER_H__
#define __CLAP_WRAPPER_H__

#ifdef __CLAP__

#include "clap_lib/clap.h"
#include "core/synth.h"
#include "core/effects.h"

/* Synth CLAP plugin structure */
typedef struct
{
	clap_plugin_t plugin;
	const clap_host_t *host;
	double sample_rate;
	synth_t synth;
} synth_plugin_t;

/* Synth CLAP plugin features */
static const char *__features[] =
{
	CLAP_PLUGIN_FEATURE_INSTRUMENT,
	CLAP_PLUGIN_FEATURE_SYNTHESIZER,
	NULL
};

/* Synth CLAP plugin descriptors */
static const clap_plugin_descriptor_t __descriptor =
{
	.clap_version = CLAP_VERSION_INIT,
	.id = "com.example.midi-synth",
	.name = "Raygui Synth - CLAP Version",
	.vendor = "gpasques-gh",
	.url = "github.com/gpasques-gh/ALSA_raygui_Synthesizer.git",
	.manual_url = "",
	.support_url = "",
	.version = "1.0.0",
	.description = "Minimal CLAP MIDI Synth",
	.features = features
};

/* Function declarations */
static uint32_t note_ports_count(
	const clap_plugin_t *plugin, 
	bool is_input);

static bool note_ports_get(
	const clap_plugin_t *plugin,
	uint32_t index,
	bool is_input,
	clap_note_port_info_t *info);

static const uint32_t audio_ports_count(
	const clap_plugin_t *plugin, 
	bool is_input);

static bool audio_ports_get(
	const clap_plugin_t *plugin,
	uint32_t index,
	bool is_input,
	clap_audio_port_info_t *info);

static void process_event(
	synth_plugin_t *p,
	const clap_eventh_header_t *hdr);

static clap_process_status plugin_process(
	const clap_plugin_t *plugin,
	const clap_process_t *process);

/* Plugin functions */
static bool plugin_init(const clap_plugin_t *plugin);
static void plugin_destroy(const clap_plugin_t *plugin);
static bool plugin_activate(
	const clap_plugin_t *plugin, 
	double sample_rate,
	uint32_t min_frames, 
	uint32_t max_frames);
static void plugin_deactivate(const clap_plugin_t *plugin);
static bool plugin_start_processing(const clap_plugin_t *plugin);
static void plugin_stop_processing(const clap_plugin_t *plugin);
static void plugin_reset(const clap_plugin_t *plugin);
static void plugin_on_main_thread(const clap_plugin_t *plugin);
static const void *plugin_get_extension(
	const clap_plugin_t *plugin, const char *id);

/* Plugin factory */
static const clap_plugin_t *create_plugin_instance(
	const clap_plugin_factory_t *factory,
	const clap_host_t *host,
	const char *plugin_id);

static uint32_t factory_get_plugin_count(
	const clap_plugin_factory_t *factory);

static const clap_plugin_descriptor_t *factory_get_plugin_descriptor(
	const clap_plugin_factory_t *factory,
	uint32_t index);

/* Plugin entry point */
static bool entry_init(const char *plugin_path);
static void entry_deinit(void);
static const void *entry_get_factory(const char *factory_id);

#endif 

#endif 