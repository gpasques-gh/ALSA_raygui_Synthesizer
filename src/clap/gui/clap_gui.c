#ifdef __CLAP__

#include <string.h>

#include "clap_lib/clap.h"
#include "clap/clap_plugin.h"
#include "clap/gui/clap_gui.h"

static void plugin_paint_rec(uint32_t *bits, rectangle_t rec)
{
	for (uint32_t y = rec.top; y < rec.bottom; y++)
	{
		for (uint32_t x = rec.left; x < rec.right; x++)
		{
			bits[y * GUI_WIDTH + x] =  (
				y == rec.top || 
				y == rec.bottom - 1 || 
				x == rec.left ||
				x == rec.right - 1)
					? rec.border_color
					: rec.fill_color;
		}
	}
}

static void plugin_paint_slider(uint32_t *bits, slider_t slider)
{
	plugin_paint_rec(bits, slider.rec_value);
	plugin_paint_rec(bits, slider.rec);
}

void plugin_paint(synth_plugin_t *plugin, uint32_t *bits) 
{
	rectangle_t background = 
	{
		.left = 0, .right = GUI_WIDTH,
		.top = 0, .bottom = GUI_HEIGHT,
		.border_color = 0x000000, .fill_color = 0x000000
	};

	plugin_paint_rec(bits, background);
	plugin->gui->elements.volume_slider.param_value = plugin->synth.amp;
	plugin_paint_slider(bits, plugin->gui->elements.volume_slider);
}

void plugin_process_mouse_drag(synth_plugin_t *plugin, int x, int y)
{

}
void plugin_process_mouse_press(synth_plugin_t *plugin, int x, int y) {}
void plugin_process_mouse_release(synth_plugin_t *plugin) {}

/* Check wether current API is supported */
bool is_api_supported(
	const clap_plugin_t *plugin, 
	const char *api, 
	bool is_floating)
{
	return !strcmp(api, GUI_API) && !is_floating;
}

/* Get the prefered API */
bool get_prefered_api(
	const clap_plugin_t *plugin, 
	const char **api, 
	bool *is_floating)
{
	(void)plugin;
	*api = GUI_API;
	*is_floating = false;
	return true;
}

/* Create the GUI with the OS specific creation function */
bool create(const clap_plugin_t *plugin, const char *api, bool is_floating)
{
	if (!is_api_supported(plugin, api, is_floating))
		return false;
	gui_create(plugin->plugin_data);
	return true;
}

void destroy(const clap_plugin_t *plugin)
{
	gui_destroy((synth_plugin_t *)plugin->plugin_data);
}

bool set_scale(const clap_plugin_t *plugin, double scale)
{
	(void)plugin; (void)scale;
	return false;
}

bool get_size(
	const clap_plugin_t *plugin, 
	uint32_t *w, uint32_t *h)
{
	(void)plugin;
	*w = GUI_WIDTH;
	*h = GUI_HEIGHT;
	return true;
}

bool can_resize(const clap_plugin_t *plugin)
{
	(void)plugin;
	return false;
}

bool get_resize_hints(
	const clap_plugin_t *plugin, 
	clap_gui_resize_hints_t *hints)
{
	(void)plugin; (void)hints;
	return false;
}

bool adjust_size(
	const clap_plugin_t *plugin, 
	uint32_t *w, uint32_t *h)
{
	return get_size(plugin, w, h);
}

bool set_size(
	const clap_plugin_t *plugin, 
	uint32_t w, uint32_t h)
{
	(void)plugin; (void)w; (void)h; 
	return true;
}

bool set_parent(
	const clap_plugin_t *plugin, 
	const clap_window_t *window)
{
	gui_set_parent((synth_plugin_t *)plugin->plugin_data, window);
	return true;
}

bool set_transient(
	const clap_plugin_t *plugin, 
	const clap_window_t *window)
{
	(void)plugin; (void)window;
	return false;
}

void suggest_title(const clap_plugin_t *plugin, const char *title) { }

bool show(const clap_plugin_t *plugin)
{
	gui_set_visible((synth_plugin_t *)plugin->plugin_data, true);
	return true;
}

bool hide(const clap_plugin_t *plugin)
{
	gui_set_visible((synth_plugin_t *)plugin->plugin_data, false);
	return true;
}

const clap_plugin_gui_t gui_ext =
{
	.is_api_supported = is_api_supported,
	.get_preferred_api = get_prefered_api,
	.create = create,
	.destroy = destroy,
	.set_scale = set_scale,
	.get_size = get_size,
	.can_resize = can_resize,
	.get_resize_hints = get_resize_hints,
	.adjust_size = adjust_size,
	.set_size = set_size,
	.set_parent = set_parent,
	.set_transient = set_transient,
	.suggest_title = suggest_title,
	.show = show,
	.hide = hide
};

#endif 