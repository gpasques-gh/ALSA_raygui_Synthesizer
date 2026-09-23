#ifdef __CLAP__

#ifndef __CLAP_GUI_H__
#define __CLAP_GUI_H__

#define GUI_WIDTH (300)
#define GUI_HEIGHT (200)

#ifdef __linux__
#include "clap/gui/clap_gui_linux.h"
#elif defined(_WIN32)
#include "clap/gui/clap_gui_windows.h"
#endif 

#include "clap/clap_plugin.h"

typedef struct synth_plugin_s synth_plugin_t;


void plugin_paint(synth_plugin_t *plugin, uint32_t *bits);
void plugin_process_mouse_drag(synth_plugin_t *plugin, int x, int y);
void plugin_process_mouse_press(synth_plugin_t *plugin, int x, int y);
void plugin_process_mouse_release(synth_plugin_t *plugin);

#endif /* __CLAP_GUI_H__ */
#endif /* __CLAP__*/