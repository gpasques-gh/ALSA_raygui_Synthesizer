#ifndef __CLAP_NOTE_PORTS_H__
#define __CLAP_NOTE_PORTS_H__

#ifdef __CLAP__

#include "lib_clap/include/clap/clap.h"

/* Function declarations */
uint32_t note_ports_count(
	const clap_plugin_t *plugin, 
	bool is_input);

bool note_ports_get(
	const clap_plugin_t *plugin,
	uint32_t index,
	bool is_input,
	clap_note_port_info_t *info);

#endif /* __CLAP__*/
#endif /* __CLAP_NOTE_PORTS_H__ */