#pragma once
#include "input_backend.h"
#include "oscillator.h"
#include "stdint.h"
#include "synth_utils.h"
#include <stddef.h>

typedef struct {
    double sample_rate;
    double tuning;
    double volume;
    Oscillator *osc;
    uint8_t num_osc;
    KeyboardState *kb_state;
} SynthState;

void synth_init(SynthState *s, KeyboardState *kb, double sample_rate,
                double tuning, double volume, uint8_t num_osc);
void synth_update(SynthState *s);
float synth_render(SynthState *s);
