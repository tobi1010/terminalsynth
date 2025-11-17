#include "audio_backend.h"
#include "input_backend.h"
#include "synth.h"
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

static AudioBackendConfig config = {
    .block_size = 512, .channels = 2, .sample_rate = 44100};

static int audio_callback(float **buffer, int frames, void *user_data) {
    SynthState *s = user_data;

    for (int i = 0; i < frames; i++) {
        float sample = synth_render(s);
        buffer[0][i] = buffer[1][i] = sample;
    }
    return 0;
}

int main(void) {
    // disable_terminal_echo();
    KeyboardState kb_state = {0};
    SynthState synth;
    synth_init(&synth, &kb_state, config.sample_rate, 440.0, 0.5, 8);

    input_backend_init(&kb_state);
    // audio_backend_init(&config);
    // audio_backend_set_callback(audio_callback, &synth);
    // audio_backend_start();
    while (1) {
        input_backend_poll();
        // synth_update(&synth);
        // usleep(1000);
    }
    audio_backend_stop();
    audio_backend_shutdown();
    input_backend_shutdown();
    // enable_terminal_echo();
}
