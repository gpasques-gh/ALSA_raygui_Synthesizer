#ifndef __CLAP_FACTORY_H__
#define __CLAP_FACTORY_H__

#ifdef __CLAP__

#include "clap_lib/clap.h"

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

#endif /* __CLAP__ */
#endif /* __CLAP_FACTORY_H__ */