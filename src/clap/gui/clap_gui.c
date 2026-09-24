#ifdef __CLAP__

#include <string.h>
#include <float.h> 

#include "clap_lib/clap.h"
#include "clap/clap_plugin.h"
#include "clap/gui/clap_gui.h"

#define IN_REC(x, y, rec) (x >= (rec.left) && x < (rec.right) && y >= (rec.top) && y < (rec.bottom))

/* Send which param corresponds to a XY pos on the GUI */
static uint32_t get_param_gui(
	gui_elements_t elements, 
	int x, int y)
{
	/* SLIDERS */

	/* Amplification */
	rectangle_t amp = 
		elements.volume_slider.rec_value;
	
	/* ADSR */
	rectangle_t attack =
		elements.adsr_sliders[0].rec_value;
	rectangle_t decay =
		elements.adsr_sliders[1].rec_value;
	rectangle_t sustain =
		elements.adsr_sliders[2].rec_value;
	rectangle_t release =
		elements.adsr_sliders[3].rec_value;

	/* Filter ADSR */
	rectangle_t f_attack =
		elements.filter_adsr_sliders[0].rec_value;
	rectangle_t f_decay =
		elements.filter_adsr_sliders[1].rec_value;
	rectangle_t f_sustain =
		elements.filter_adsr_sliders[2].rec_value;
	rectangle_t f_release =
		elements.filter_adsr_sliders[3].rec_value;

	/* Filter cutoff */
	rectangle_t cutoff = 
		elements.cutoff_slider.rec_value;

	if (IN_REC(x, y, amp))
		return P_VOLUME;
	if (IN_REC(x, y, attack))
		return P_ATTACK;
	if (IN_REC(x, y, decay))
		return P_DECAY;
	if (IN_REC(x, y, sustain))
		return P_SUSTAIN;
	if (IN_REC(x, y, release))
		return P_RELEASE;
	if (IN_REC(x, y, f_attack))
		return P_FILTER_ATTACK;
	if (IN_REC(x, y, f_decay))
		return P_FILTER_DECAY;
	if (IN_REC(x, y, f_sustain))
		return P_FILTER_SUSTAIN;
	if (IN_REC(x, y, f_release))
		return P_FILTER_RELEASE;
	if (IN_REC(x, y, cutoff))
		return P_CUTOFF;

	return P_COUNT;
} 

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

static rectangle_t get_slider_rec(gui_elements_t elements, uint32_t param_id)
{
	switch (param_id)
	{
	case P_VOLUME: return elements.volume_slider.rec;
	case P_ATTACK: return elements.adsr_sliders[0].rec;
	case P_DECAY: return elements.adsr_sliders[1].rec;
	case P_SUSTAIN: return elements.adsr_sliders[2].rec;
	case P_RELEASE: return elements.adsr_sliders[3].rec;
	case P_FILTER_ATTACK: return elements.filter_adsr_sliders[0].rec;
	case P_FILTER_DECAY: return elements.filter_adsr_sliders[1].rec;
	case P_FILTER_SUSTAIN: return elements.filter_adsr_sliders[2].rec;
	case P_FILTER_RELEASE: return elements.filter_adsr_sliders[3].rec;
	case P_CUTOFF: return elements.cutoff_slider.rec;
	default: return (rectangle_t){0};
	}
}

static rectangle_t compute_horizontal_slider_rec(
	rectangle_t main_rec, uint32_t width,
	float value, float max)
{
	value = value / max;
	if (value < 0.0f) value = 0.0f;
	if (value > 1.0f) value = 1.0f;

	uint32_t travel = (main_rec.right - main_rec.left) - width;
	return (rectangle_t)
	{
		.top = main_rec.top,
		.bottom = main_rec.bottom,
		.left = main_rec.left + (uint32_t)(travel * value),
		.right = main_rec.left + width + (uint32_t)(travel * value),
		.border_color = BLACK, .fill_color = GRAY
	};
}

void gui_create_elements(synth_plugin_t *plugin)
{
	/* Read the parameter store used by both the host and the GUI. The audio
	 * thread may not have copied a newly changed parameter into synth yet. */
	float amp = atomic_load(&plugin->params[P_VOLUME]);
	float attack = atomic_load(&plugin->params[P_ATTACK]);
	float decay = atomic_load(&plugin->params[P_DECAY]);
	float sustain = atomic_load(&plugin->params[P_SUSTAIN]);
	float release = atomic_load(&plugin->params[P_RELEASE]);
	float f_attack = atomic_load(&plugin->params[P_FILTER_ATTACK]);
	float f_decay = atomic_load(&plugin->params[P_FILTER_DECAY]);
	float f_sustain = atomic_load(&plugin->params[P_FILTER_SUSTAIN]);
	float f_release = atomic_load(&plugin->params[P_FILTER_RELEASE]);
	float cutoff = atomic_load(&plugin->params[P_CUTOFF]);

	/* Amplification slider */
	rectangle_t amp_rec = { 10, 110, 10, 50, BLACK, GRAY};
	plugin->gui->elements.volume_slider.rec = amp_rec;
	plugin->gui->elements.volume_slider.rec_value = 
		compute_horizontal_slider_rec(amp_rec, 20, amp, 1.0f);
	plugin->gui->elements.volume_slider.param_value = amp;

	/* Attack slider */
	rectangle_t attack_rec = { 10, 110, 80, 120, BLACK, GRAY};
	plugin->gui->elements.adsr_sliders[0].rec = attack_rec;
	plugin->gui->elements.adsr_sliders[0].rec_value = 
		compute_horizontal_slider_rec(attack_rec, 20, attack, 2.0f);
	plugin->gui->elements.adsr_sliders[0].param_value = attack;

	/* Decay slider */
	rectangle_t decay_rec = { 10, 110, 140, 180, BLACK, GRAY};
	plugin->gui->elements.adsr_sliders[1].rec = decay_rec;
	plugin->gui->elements.adsr_sliders[1].rec_value = 
		compute_horizontal_slider_rec(decay_rec, 20, decay, 2.0f);
	plugin->gui->elements.adsr_sliders[1].param_value = decay;

	/* Sustain slider */
	rectangle_t sustain_rec = { 10, 110, 200, 240, BLACK, GRAY};
	plugin->gui->elements.adsr_sliders[2].rec = sustain_rec;
	plugin->gui->elements.adsr_sliders[2].rec_value = 
		compute_horizontal_slider_rec(sustain_rec, 20, sustain, 1.0f);
	plugin->gui->elements.adsr_sliders[2].param_value = sustain;

	/* Release slider */
	rectangle_t release_rec = { 10, 110, 260, 300, BLACK, GRAY};
	plugin->gui->elements.adsr_sliders[3].rec = release_rec;
	plugin->gui->elements.adsr_sliders[3].rec_value = 
		compute_horizontal_slider_rec(release_rec, 20, release, 2.0f);
	plugin->gui->elements.adsr_sliders[3].param_value = release;

	/* Filter Attack slider */
	rectangle_t f_attack_rec = { 140, 240, 80, 120, BLACK, GRAY};
	plugin->gui->elements.filter_adsr_sliders[0].rec = f_attack_rec;
	plugin->gui->elements.filter_adsr_sliders[0].rec_value = 
		compute_horizontal_slider_rec(f_attack_rec, 20, f_attack, 2.0f);
	plugin->gui->elements.filter_adsr_sliders[0].param_value = f_attack;

	/* Filter Decay slider */
	rectangle_t f_decay_rec = { 140, 240, 140, 180, BLACK, GRAY};
	plugin->gui->elements.filter_adsr_sliders[1].rec = f_decay_rec;
	plugin->gui->elements.filter_adsr_sliders[1].rec_value = 
		compute_horizontal_slider_rec(f_decay_rec, 20, f_decay, 2.0f);
	plugin->gui->elements.filter_adsr_sliders[1].param_value = f_decay;

	/* Filter Sustain slider */
	rectangle_t f_sustain_rec = { 140, 240, 200, 240, BLACK, GRAY};
	plugin->gui->elements.filter_adsr_sliders[2].rec = f_sustain_rec;
	plugin->gui->elements.filter_adsr_sliders[2].rec_value = 
		compute_horizontal_slider_rec(f_sustain_rec, 20, f_sustain, 1.0f);
	plugin->gui->elements.filter_adsr_sliders[2].param_value = f_sustain;

	/* Filter Release slider */
	rectangle_t f_release_rec = { 140, 240, 260, 300, BLACK, GRAY};
	plugin->gui->elements.filter_adsr_sliders[3].rec = f_release_rec;
	plugin->gui->elements.filter_adsr_sliders[3].rec_value = 
		compute_horizontal_slider_rec(f_release_rec, 20, f_release, 2.0f);
	plugin->gui->elements.filter_adsr_sliders[3].param_value = f_release;

	/* Cutoff slider */
	rectangle_t cutoff_rec = { 140, 240, 320, 360, BLACK, GRAY};
	plugin->gui->elements.cutoff_slider.rec = cutoff_rec;
	plugin->gui->elements.cutoff_slider.rec_value = 
		compute_horizontal_slider_rec(cutoff_rec, 20, cutoff, 1.0f);
	plugin->gui->elements.cutoff_slider.param_value = cutoff;
}

static void plugin_paint_slider(uint32_t *bits, slider_t slider)
{
	plugin_paint_rec(bits, slider.rec);
	plugin_paint_rec(bits, slider.rec_value);
}

static void update_sliders(synth_plugin_t *p)
{
	/* Get the new data */
	float amp = atomic_load(&p->params[P_VOLUME]);
	float attack = atomic_load(&p->params[P_ATTACK]);
	float decay = atomic_load(&p->params[P_DECAY]);
	float sustain = atomic_load(&p->params[P_SUSTAIN]);
	float release = atomic_load(&p->params[P_RELEASE]);
	float f_attack = atomic_load(&p->params[P_FILTER_ATTACK]);
	float f_decay = atomic_load(&p->params[P_FILTER_DECAY]);
	float f_sustain = atomic_load(&p->params[P_FILTER_SUSTAIN]);
	float f_release = atomic_load(&p->params[P_FILTER_RELEASE]);
	float cutoff = atomic_load(&p->params[P_CUTOFF]);

	/* Update the volume slider */
	p->gui->elements.volume_slider.param_value = amp;
	p->gui->elements.volume_slider.rec_value =
		compute_horizontal_slider_rec(
			p->gui->elements.volume_slider.rec, 20,
			amp, 1.0f);

	/* Update the attack slider */
	p->gui->elements.adsr_sliders[0].param_value = attack;
	p->gui->elements.adsr_sliders[0].rec_value =
		compute_horizontal_slider_rec(
			p->gui->elements.adsr_sliders[0].rec, 20,
			attack, 2.0f);

	/* Update the decay slider */
	p->gui->elements.adsr_sliders[1].param_value = decay;
	p->gui->elements.adsr_sliders[1].rec_value =
		compute_horizontal_slider_rec(
			p->gui->elements.adsr_sliders[1].rec, 20,
			decay, 2.0f);

	/* Update the sustain slider */
	p->gui->elements.adsr_sliders[2].param_value = sustain;
	p->gui->elements.adsr_sliders[2].rec_value =
		compute_horizontal_slider_rec(
			p->gui->elements.adsr_sliders[2].rec, 20,
			sustain, 1.0f);

	/* Update the release slider */
	p->gui->elements.adsr_sliders[3].param_value = release;
	p->gui->elements.adsr_sliders[3].rec_value =
		compute_horizontal_slider_rec(
			p->gui->elements.adsr_sliders[3].rec, 20,
			release, 2.0f);

	/* Update the filter attack slider */
	p->gui->elements.filter_adsr_sliders[0].param_value = f_attack;
	p->gui->elements.filter_adsr_sliders[0].rec_value =
		compute_horizontal_slider_rec(
			p->gui->elements.filter_adsr_sliders[0].rec, 20,
			f_attack, 2.0f);

	/* Update the filter decay slider */
	p->gui->elements.filter_adsr_sliders[1].param_value = f_decay;
	p->gui->elements.filter_adsr_sliders[1].rec_value =
		compute_horizontal_slider_rec(
			p->gui->elements.filter_adsr_sliders[1].rec, 20,
			f_decay, 2.0f);

	/* Update the filter sustain slider */
	p->gui->elements.filter_adsr_sliders[2].param_value = f_sustain;
	p->gui->elements.filter_adsr_sliders[2].rec_value =
		compute_horizontal_slider_rec(
			p->gui->elements.filter_adsr_sliders[2].rec, 20,
			f_sustain, 1.0f);

	/* Update the filter release slider */
	p->gui->elements.filter_adsr_sliders[3].param_value = f_release;
	p->gui->elements.filter_adsr_sliders[3].rec_value =
		compute_horizontal_slider_rec(
			p->gui->elements.filter_adsr_sliders[3].rec, 20,
			f_release, 2.0f);

	/* Update the cutoff slider */
	p->gui->elements.cutoff_slider.param_value = cutoff;
	p->gui->elements.cutoff_slider.rec_value =
		compute_horizontal_slider_rec(
			p->gui->elements.cutoff_slider.rec, 20,
			cutoff, 1.0f);
}
void plugin_paint(synth_plugin_t *plugin, uint32_t *bits) 
{
	rectangle_t background = 
	{
		.left = 0, .right = GUI_WIDTH,
		.top = 0, .bottom = GUI_HEIGHT,
		.border_color = BLACK, .fill_color = BLACK
	};
	
	plugin_paint_rec(bits, background);
	
	update_sliders(plugin);

	/* Painting amplification slider */
	plugin_paint_slider(bits, plugin->gui->elements.volume_slider);

	/* Painting ADSR sliders */
	plugin_paint_slider(bits, plugin->gui->elements.adsr_sliders[0]);
	plugin_paint_slider(bits, plugin->gui->elements.adsr_sliders[1]);
	plugin_paint_slider(bits, plugin->gui->elements.adsr_sliders[2]);
	plugin_paint_slider(bits, plugin->gui->elements.adsr_sliders[3]);

	/* Painting filter ADSR sliders and cutoff */
	plugin_paint_slider(bits, plugin->gui->elements.filter_adsr_sliders[0]);
	plugin_paint_slider(bits, plugin->gui->elements.filter_adsr_sliders[1]);
	plugin_paint_slider(bits, plugin->gui->elements.filter_adsr_sliders[2]);
	plugin_paint_slider(bits, plugin->gui->elements.filter_adsr_sliders[3]);
	plugin_paint_slider(bits, plugin->gui->elements.cutoff_slider);
}

void plugin_process_mouse_drag(synth_plugin_t *plugin, int x, int y)
{
	(void)y;
	if (plugin->mouse.mouse_dragging)
	{
		/* Rectangle data */
		uint32_t id = plugin->mouse.drag_param_id;
		rectangle_t slider = get_slider_rec(plugin->gui->elements, id);
		const uint32_t handle_width = 20;

		/* Calculate the new value from the drag */
		float travel = (float)((slider.right - slider.left) - handle_width);
		float new_val = (x - (float)slider.left) / travel;
		if (new_val < 0.0f) new_val = 0.0f;
		if (new_val > 1.0f) new_val = 1.0f;

		/* Double the value if slider is 0.0 to 2.0 range */
		bool is_2_range =
			id == P_ATTACK || id == P_DECAY || id == P_RELEASE ||
			id == P_FILTER_ATTACK || id == P_FILTER_DECAY || id == P_FILTER_RELEASE;
		new_val *= is_2_range ? 2.0f : 1.0f;

		atomic_store(&plugin->params[plugin->mouse.drag_param_id], new_val);
		atomic_store(&plugin->params_dirty[plugin->mouse.drag_param_id], true);

		if (plugin->host_params && plugin->host_params->request_flush)
			plugin->host_params->request_flush(plugin->host);
	}
}

void plugin_process_mouse_press(synth_plugin_t *plugin, int x, int y)
{
	uint32_t param_id = get_param_gui(plugin->gui->elements, x, y);
	if (param_id >= 0 && param_id < P_COUNT)
	{
		plugin->mouse.mouse_dragging = true;
		plugin->mouse.drag_param_id = param_id;
		plugin->mouse.mouse_drag_og_x = x;
		plugin->mouse.mouse_drag_og_y = y;
		plugin->mouse.drag_param_og_val = atomic_load(&plugin->params[plugin->mouse.drag_param_id]);
		atomic_store(&plugin->gestures_start[plugin->mouse.drag_param_id], true);

		if (plugin->host_params && plugin->host_params->request_flush)
			plugin->host_params->request_flush(plugin->host);
	}
}

void plugin_process_mouse_release(synth_plugin_t *plugin)
{
	if (plugin->mouse.mouse_dragging)
	{
		atomic_store(&plugin->gestures_end[plugin->mouse.drag_param_id], true);
		if (plugin->host_params && plugin->host_params->request_flush)
			plugin->host_params->request_flush(plugin->host);
		plugin->mouse.mouse_dragging = false;
	}
}

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
