#ifndef __CLAP_PLUGIN_H__
#define __CLAP_PLUGIN_H__

#ifdef __CLAP__

#include "clap_lib/clap.h"
#include "core/synth.h"

/* Synth CLAP plugin structure */
typedef struct
{
	clap_plugin_t plugin;
	const clap_host_t *host;
	double sample_rate;
	synth_t synth;
} synth_plugin_t;

static void process_event(
	synth_plugin_t *p,
	const clap_event_header_t *hdr);

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

#endif /* __CLAP__ */
#endif /* __CLAP_PLUGIN_H__ */