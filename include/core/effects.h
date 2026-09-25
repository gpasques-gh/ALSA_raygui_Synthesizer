#ifndef EFFECTS_H
#define EFFECTS_H

#include <stdbool.h>

/* Applies an amount of distortion onto a sound buffer */
short distortion(short sample, float amount, bool overdriving);

#endif 