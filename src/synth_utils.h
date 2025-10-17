#pragma once
#include <math.h>
static inline double midi_to_freq(int midi_note, double tuning) {
    return tuning * pow(2.0, (midi_note - 69) / 12.0);
}
