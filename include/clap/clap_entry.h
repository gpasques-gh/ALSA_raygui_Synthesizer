#ifndef __CLAP_WRAPPER_H__
#define __CLAP_WRAPPER_H__

#ifdef __CLAP__

#include "lib_clap/include/clap/clap.h"

/* Plugin entry point */
bool entry_init(const char *plugin_path);
void entry_deinit(void);
const void *entry_get_factory(const char *factory_id);

#endif /* __CLAP__ */
#endif /* __CLAP_WRAPPER_H__ */