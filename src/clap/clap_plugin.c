#ifdef __CLAP__

#include "clap/clap_plugin.h"
#include "core/synth.h"
#include "core/effects.h"

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

static void process_event(
	synth_plugin_t *p,
	const clap_event_header_t *hdr)
{
	if (hdr->space_id == CLAP_CORE_EVENT_SPACE_ID)
	{

	}
}

#endif 