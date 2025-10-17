#ifndef AUDIO_BACKEND_H
#define AUDIO_BACKEND_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int sample_rate;
    int block_size;
    int channels;
} AudioBackendConfig;

typedef int (*AudioCallback)(float **buffer, int frames, void *user_data);

int audio_backend_init(const AudioBackendConfig *cfg);
void audio_backend_set_callback(AudioCallback cb, void *user_data);
void audio_backend_start(void);
void audio_backend_stop(void);
void audio_backend_shutdown(void);

#ifdef __cplusplus
}
#endif
#endif
