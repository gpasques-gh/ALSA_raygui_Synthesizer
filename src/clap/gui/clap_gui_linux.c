#ifdef __linux__

#include "clap/clap_plugin.h"
#include "clap/gui/clap_gui_linux.h"

void gui_create(synth_plugin_t *plugin)
{
    /* Allocate the GUI */
    plugin->gui = (clap_gui_t *)calloc(1, sizeof(clap_gui_t));

    /* Open the X11 display and window */
    plugin->gui->display = XOpenDisplay(NULL);
    XSetWindowAttributes attributes = {0};
    plugin->gui->window = XCreateWindow(
        plugin->gui->display, 
        DefaultRootWindow(plugin->gui->display),
        0, 0, GUI_WIDTH, GUI_HEIGHT, 0, 0, 
        InputOutput, CopyFromParent, CWOverrideRedirect, &attributes);
    XStoreName(plugin->gui->display, plugin->gui->window, plugin);
}

#endif