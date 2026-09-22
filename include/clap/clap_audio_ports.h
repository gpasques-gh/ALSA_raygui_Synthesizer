#ifndef __CLAP_WRAPPER_H__
#define __CLAP_WRAPPER_H__

#ifdef __CLAP__

#include "clap_lib/clap.h"

static const uint32_t audio_ports_count(
	const clap_plugin_t *plugin, 
	bool is_input);

static bool audio_ports_get(
	const clap_plugin_t *plugin,
	uint32_t index,
	bool is_input,
	clap_audio_port_info_t *info);

#endif 

#endif 