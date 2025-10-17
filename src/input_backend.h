#ifndef INPUT_BACKEND_H
#define INPUT_BACKEND_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
#define MAX_KEYS 128

typedef struct {
    uint8_t keys[MAX_KEYS];
    uint8_t prev[MAX_KEYS];
    uint8_t num_keys_pressed;
} KeyboardState;

void input_backend_init(KeyboardState *state);
void input_backend_shutdown(void);
void input_backend_poll(void);
void disable_terminal_echo(void);
void enable_terminal_echo(void);
#ifdef __cplusplus
}
#endif
#endif
