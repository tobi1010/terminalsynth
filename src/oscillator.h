#pragma once
#include <math.h>
#include <stdint.h>

#define TABLE_SIZE 2048

typedef enum {
    WAVE_SINE,
    WAVE_SQUARE,
    WAVE_SAW,
    WAVE_TRIANGLE,
} WaveformType;

typedef struct {
    double phase;
    double freq;
    double amp;
    WaveformType wave;
    const float *table;
    uint8_t note;
} Oscillator;

void osc_init(Oscillator *o, WaveformType wave, double freq, double amp);
float osc_next(Oscillator *o, double sample_rate);
