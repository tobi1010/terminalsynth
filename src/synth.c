#include "synth.h"
#include "input_backend.h"
#include "oscillator.h"
#include "synth_utils.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void synth_init(SynthState *s, KeyboardState *kb_state, double sample_rate,
                double tuning, double volume, uint8_t num_osc) {
    s->sample_rate = sample_rate;
    s->tuning = tuning;
    s->kb_state = kb_state;
    s->volume = volume;
    s->num_osc = num_osc;
    s->osc = malloc(sizeof(Oscillator) * s->num_osc);
    for (int i = 0; i < s->num_osc; i++) {
        osc_init(&s->osc[i], WAVE_SINE, tuning, 0.0);
    }
}

void synth_update(SynthState *s) {
    KeyboardState *kb = s->kb_state;
    for (int i = 0; i < (int)key_note_map_len; i++) {
        uint8_t was = kb->prev[key_note_map[i].key];
        uint8_t now = kb->keys[key_note_map[i].key];
        if (now && !was) {
            for (int j = 0; j < s->num_osc; j++) {
                if (s->osc[j].amp == 0.0) {
                    s->osc[j].freq =
                        midi_to_freq(key_note_map[i].midi_note, s->tuning);
                    s->osc[j].amp = s->volume;
                    s->osc[j].note = key_note_map[i].midi_note;
                    printf("%d", j);
                    break;
                }
            }
        }
        if (!now && was) {
            for (int j = 0; j < s->num_osc; j++) {
                if (s->osc[j].note == key_note_map[i].midi_note) {
                    s->osc[j].freq =
                        midi_to_freq(key_note_map[i].midi_note, s->tuning);
                    s->osc[j].amp = 0.0;
                    s->osc[j].note = 0;
                    break;
                }
            }
        }

        kb->prev[key_note_map[i].key] = now;
    }
}

float synth_render(SynthState *s) {
    float sum = 0;
    for (int i = 0; i < s->num_osc; i++) {
        sum += osc_next(&s->osc[i], s->sample_rate);
    }
    return sum / s->num_osc;
}
void synth_shutdown(SynthState *s) {
    free(s->osc);
    s->osc = NULL;
}
