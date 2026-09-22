#ifdef __CLAP__

#include "clap/clap_params.h"
#include "defs.h"

const param_desc_t PARAMS[P_COUNT] =
{
	PARAM_VOLUME,
	PARAM_WAVE_A,
	PARAM_WAVE_B,
	PARAM_WAVE_C,
	PARAM_DETUNE,
	PARAM_ATTACK,
	PARAM_DECAY,
	PARAM_SUSTAIN,
	PARAM_RELEASE,
	PARAM_CUTOFF
};

const param_desc_t *param_desc_from_id(clap_id id)
{
	return id < P_COUNT ? &PARAMS[id] : NULL;
}

static bool valid_param(clap_id id)
{
	return id < P_COUNT;
}

static uint32_t params_count(const clap_plugin_t *plugin)
{
	(void)plugin;
	return P_COUNT;
}

static bool params_get_info(
	const clap_plugin_t *plugin,
	uint32_t index,
	clap_param_info_t *info)
{
	(void)plugin;

	if (index >= P_COUNT || !info)
		return false;

	const param_desc_t *src = &PARAMS[index];
	memset(info, 0, sizeof(*info));

	info->id = src->id;
	info->flags = src->flags;
	info->cookie = NULL;
	snprintf(info->name, sizeof(info->name), "%s", src->name);
	snprintf(info->module, sizeof(info->module), "%s", src->module);
	info->min_value = src->min;
	info->max_value = src->max;
	info->default_value = src->def;
	return true;
}

static bool params_get_value(
	const clap_plugin_t *plugin,
	clap_id id,
	double *out_value)
{
	if (!valid_param(id) || !out_value)
		return false;

	synth_plugin_t *p = plugin->plugin_data;
	*out_value = atomic_load(&p->params[id]);
	return true;
}

/* Get the parameter text from its ID */
static bool params_value_to_text(
	const clap_plugin_t *plugin,
	clap_id id,
	double value,
	char *out,
	uint32_t capacity)
{
	(void)plugin;

	if (!valid_param(id) || !out || capacity == 0)
		return false;
	
	if (id >= P_WAVE_A && id <= P_WAVE_C)
		snprintf(out, capacity, "%s", get_wave_name((int)value));
	else if (id == P_ATTACK || id == P_DECAY || id == P_RELEASE)
		snprintf(out, capacity, "%.3f s", value);
	else
		snprintf(out, capacity, "%.2f", value);
	
	return true;
}

/* Get the parameter value from its text */
static bool params_text_to_value(
	const clap_plugin_t *plugin,
	clap_id id,
	const char *text,
	double *out_value)
{
	(void)plugin;

	if (!valid_param(id) || !text || !out_value)
		return false;

	/* Waveforms */
	if (!strcmp(text, "Sine"))     { *out_value = SINE_WAVE; return true; }
    if (!strcmp(text, "Square"))   { *out_value = SQUARE_WAVE; return true; }
    if (!strcmp(text, "Triangle")) { *out_value = TRIANGLE_WAVE; return true; }
    if (!strcmp(text, "Sawtooth")) { *out_value = SAWTOOTH_WAVE; return true; }

	/* Parse the value from string to double */
	char *end = NULL;
	double value = strtod(text, &end);
	if (end == text)
		return false;

	/* Clamping */
	if (value < PARAMS[id].min) value = PARAMS[id].min;
	if (value > PARAMS[id].max) value = PARAMS[id].max;

	*out_value = value;
	return true;
}

/* Send the parameters to the audio thread */
static void params_flush(
	const clap_plugin_t *plugin,
	const clap_input_events_t *in,
	const clap_output_events_t *out)
{
	(void)out;

	synth_plugin_t *p = plugin->plugin_data;
	uint32_t count = in->size(in);

	for (int i = 0; i < count; i++)
	{
		const clap_event_header_t *event = in->get(in, i);
		if (event->space_id == CLAP_CORE_EVENT_SPACE_ID && 
			event->type == CLAP_EVENT_PARAM_VALUE)
				process_event(p, event);
	}
}

/* Parameters extension */
const clap_plugin_params_t params_ext =
{
	.count = params_count,
	.get_info = params_get_info,
	.get_value = params_get_value,
	.value_to_text = params_value_to_text,
	.text_to_value = params_text_to_value,
	.flush = params_flush,
};

#endif 