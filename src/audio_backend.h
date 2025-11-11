#ifndef AUDIO_BACKEND_H
#define AUDIO_BACKEND_H

#include <stdint.h>

typedef struct {
    uint32_t sample_rate;
    uint32_t block_size;
    uint32_t channels;
} AudioBackendConfig;

typedef int (*AudioCallback)(float **buffer, int frames, void *user_data);

int audio_backend_init(const AudioBackendConfig *cfg);
void audio_backend_set_callback(AudioCallback cb, void *user_data);
void audio_backend_start(void);
void audio_backend_stop(void);
void audio_backend_shutdown(void);

#endif
