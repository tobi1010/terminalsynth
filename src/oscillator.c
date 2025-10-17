#include "oscillator.h"
#include <stdbool.h>
#include <stddef.h>

static float sine_table[TABLE_SIZE];
static float square_table[TABLE_SIZE];
static float saw_table[TABLE_SIZE];

static bool tables_initialized = false;

static void init_tables(void) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        float phase = (float)i / TABLE_SIZE;
        sine_table[i] = sinf(2.0f * M_PI * phase);
        square_table[i] = phase < 0.5f ? 1.0f : -1.0f;
        saw_table[i] = 2.0f * phase - 1.0f;
    }
    tables_initialized = true;
}

void osc_init(Oscillator *o, WaveformType wave, double freq, double amp) {
    if (!tables_initialized) {
        init_tables();
    }
    o->phase = 0.0;
    o->freq = freq;
    o->amp = amp;
    o->wave = wave;
    o->note = 0.0;

    switch (wave) {
    case WAVE_SINE:
        o->table = sine_table;
        break;
    case WAVE_SQUARE:
        o->table = square_table;
        break;
    case WAVE_SAW:
        o->table = saw_table;
        break;
    default:
        o->table = sine_table;
        break;
    }
}

float osc_next(Oscillator *o, double sample_rate) {
    const float *table = o->table;

    double step = (o->freq * TABLE_SIZE) / sample_rate;
    o->phase += step;

    if (o->phase >= TABLE_SIZE)
        o->phase -= TABLE_SIZE;

    double phase_pos = o->phase;

    int base_index = (int)phase_pos;
    float frac_part = (float)(phase_pos - base_index);

    int next_index = (base_index + 1) % TABLE_SIZE;

    float s0 = table[base_index];
    float s1 = table[next_index];
    float sample = s0 + frac_part * (s1 - s0);
    return sample * (float)o->amp;
}
