#ifndef __CLAP_FACTORY_H__
#define __CLAP_FACTORY_H__

#ifdef __CLAP__

#include "lib_clap/include/clap/clap.h"

/* Plugin factory */
const clap_plugin_t *create_plugin_instance(
	const clap_plugin_factory_t *factory,
	const clap_host_t *host,
	const char *plugin_id);

uint32_t factory_get_plugin_count(
	const clap_plugin_factory_t *factory);

const clap_plugin_descriptor_t *factory_get_plugin_descriptor(
	const clap_plugin_factory_t *factory,
	uint32_t index);

extern const clap_plugin_factory_t plugin_factory;

#endif /* __CLAP__ */
#endif /* __CLAP_FACTORY_H__ */