#ifndef __CLAP_WRAPPER_H__
#define __CLAP_WRAPPER_H__

#ifdef __CLAP__

#include "clap_lib/clap.h"

/* Plugin entry point */
static bool entry_init(const char *plugin_path);
static void entry_deinit(void);
static const void *entry_get_factory(const char *factory_id);

#endif /* __CLAP__ */
#endif /* __CLAP_WRAPPER_H__ */