basic synthesizer for terminal on macOs. generates a sine wave. 6 voices on standard keyboard.

compile with:
```bash
clang -std=c99 -Wall -Wextra -pedantic\
      main.c synth.c audio_backend_coreaudio.c input_backend_osx.c oscillator.c\
      -framework AudioToolbox -framework IOKit -framework CoreFoundation\
      -o synth
```
run:
```bash
./synth
```
then play with asdfghjkl; keys
