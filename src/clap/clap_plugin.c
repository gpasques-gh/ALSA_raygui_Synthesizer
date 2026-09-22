#ifdef __CLAP__

#include "clap/clap_plugin.h"
#include "defs.h"
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
	if (hdr->space_id != CLAP_CORE_EVENT_SPACE_ID)
		return;

	if (hdr->type != CLAP_EVENT_NOTE_ON &&
		hdr->type != CLAP_EVENT_NOTE_OFF &&
		hdr->type != CLAP_EVENT_NOTE_CHOKE)
			return;
	
	switch(hdr->type)
	{
	case CLAP_EVENT_NOTE_ON:
	{
		const clap_event_note_t *ev = 
			(const clap_event_note_t *)hdr;
		voice_t *free_voice = get_free_voice(&p->synth);
		if (!free_voice)
			break;

		int pressed_voices = 0;
		
		for (int v = 0; v < VOICES; v++)
		{   
			if (p->synth.voices[v].pressed)
				pressed_voices++;
			if (p->synth.voices[v].adsr->state == ENV_RELEASE && !p->synth.arp)
				p->synth.voices[v].adsr->state = ENV_IDLE;
		}

		/* Get the first free voice */
		voice_t *free_voice = get_free_voice(&p->synth);
		if (free_voice == NULL) return;

		/* Press the voice and activate it */
		free_voice->pressed = 1;
		change_freq(free_voice, ev->key, (int)(ev->velocity * 127.0f), p->synth.detune);
		if (pressed_voices == 0 && p->synth.filter->env)
			p->synth.filter->adsr->state = ENV_ATTACK;

		break;
	}
	case CLAP_EVENT_NOTE_OFF:
	{
		const clap_event_note_t *ev = 
			(const clap_event_note_t *)hdr;

		/* Count the currently pressed voices */
		int pressed_voices = 0;
		for (int v = 0; v < VOICES; v++)
			if (p->synth.voices[v].pressed)
				pressed_voices++;
		
		/* Loop through the voices to deactivate 
		the one of which MIDI note has been released */
		for (int v = 0; v < VOICES; v++)
		{
			if (p->synth.voices[v].note == ev->key && 
				p->synth.voices[v].pressed)
			{
				if (p->synth.arp && p->synth.voices[v].adsr->state != ENV_IDLE)
				{
					p->synth.voices[v].adsr->state = ENV_IDLE;
				}
				else if (!p->synth.arp &&
						p->synth.voices[v].adsr->state != ENV_RELEASE &&
						p->synth.voices[v].adsr->state != ENV_IDLE)
				{
					p->synth.voices[v].adsr->state = ENV_RELEASE;
				}
					
				p->synth.voices[v].note = -1;
				p->synth.voices[v].pressed = 0;

				break; 
			}
		}

		break;
	}
	default:
		break;
	}
}

static clap_process_status plugin_process(
	const clap_plugin_t *plugin,
	const clap_process_t *process)
{
	synth_plugin_t *p = (synth_plugin_t *)plugin->plugin_data;
	
	const uint32_t frame_count = process->frames_count;
	const uint32_t event_count = process->in_events->size(process->in_events);
	uint32_t event_index = 0;
	uint32_t next_event_frame = event_count ? 0 : frame_count;

	for (uint32_t i = 0; i < frame_count;)
	{
		while (event_index < event_count && next_event_frame == i)
		{
			const clap_event_header_t *ev = 
				process->in_events->get(process->in_events, event_index);
			if (ev->time != i)
			{
				next_event_frame = ev->time;
				break;
			}
		}
	}

}

#endif 