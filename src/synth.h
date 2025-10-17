#pragma once
#include "input_backend.h"
#include "input_keys.h"
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

typedef struct {
    uint8_t key;
    uint8_t midi_note;
} KeyNoteMap;

static const KeyNoteMap key_note_map[] = {
    {KEY_A, 60}, {KEY_W, 61},        {KEY_S, 62}, {KEY_E, 63}, {KEY_D, 64},
    {KEY_F, 65}, {KEY_T, 66},        {KEY_G, 67}, {KEY_Y, 68}, {KEY_H, 69},
    {KEY_U, 70}, {KEY_J, 71},        {KEY_K, 72}, {KEY_O, 73}, {KEY_L, 74},
    {KEY_P, 75}, {KEY_SEMICOLON, 76}};

static const size_t key_note_map_len =
    sizeof(key_note_map) / sizeof(key_note_map[0]);

void synth_init(SynthState *s, KeyboardState *kb, double sample_rate,
                double tuning, double volume, uint8_t num_osc);
void synth_update(SynthState *s);
float synth_render(SynthState *s);
