#ifdef __CLAP__

#include <string.h>
#include <stdlib.h>

#include "clap_lib/clap.h"
#include "clap/clap_factory.h"
#include "clap/clap_plugin.h"

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
	.features = __features
};

/* Plugin factory */
const clap_plugin_t *create_plugin_instance(
	const clap_plugin_factory_t *factory,
	const clap_host_t *host,
	const char *plugin_id)
{
	(void)factory;
	if (strcmp(plugin_id, __descriptor.id))
		return NULL;
	synth_plugin_t *p = (synth_plugin_t *)calloc(1, sizeof(synth_plugin_t));
	if (!p) return NULL;

	p->host = host;
	p->plugin.desc = &__descriptor;
	p->plugin.plugin_data = p;
	p->plugin.init = plugin_init;
	p->plugin.destroy = plugin_destroy;
	p->plugin.activate = plugin_activate;
	p->plugin.deactivate = plugin_deactivate;
	p->plugin.start_processing = plugin_start_processing;
	p->plugin.stop_processing = plugin_stop_processing;
	p->plugin.reset = plugin_reset;
	p->plugin.process = plugin_process;
	p->plugin.get_extension = plugin_get_extension;
	p->plugin.on_main_thread = plugin_on_main_thread;

	return &p->plugin;
}

/* Get the plugin instance count (does nothing) */
uint32_t factory_get_plugin_count(
	const clap_plugin_factory_t *factory)
{
	(void)factory;
	return 1;
}

/* Get the plugin descriptor */
const clap_plugin_descriptor_t *factory_get_plugin_descriptor(
	const clap_plugin_factory_t *factory,
	uint32_t index)
{
	(void)factory;
	return (index == 0) ? &__descriptor : NULL;
}

/* Plugin factory structure */
const clap_plugin_factory_t plugin_factory =
{
	.get_plugin_count = factory_get_plugin_count,
	.get_plugin_descriptor = factory_get_plugin_descriptor,
	.create_plugin = create_plugin_instance
};

#endif /* __CLAP__ */