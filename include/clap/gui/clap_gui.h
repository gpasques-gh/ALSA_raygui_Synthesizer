#ifdef __CLAP__

#ifndef __CLAP_GUI_H__
#define __CLAP_GUI_H__

#define GUI_WIDTH (300)
#define GUI_HEIGHT (200)

#ifdef __linux__
#include "clap/gui/clap_gui_linux.h"
#endif 

extern const clap_plugin_gui_t gui_ext;

#endif /* __CLAP_GUI_H__ */
#endif /* __CLAP__*/