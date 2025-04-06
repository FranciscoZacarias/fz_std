
# Personal standard library

Basic structure:

```c
#include "fz_include.h"

// Run once at start of program
void application_init() {
}

// Run every tick. Return false to exit.
void application_tick() {
}

```

See `example.c` for examples on how to:
  - Create and launch window (bare metal win32)
  - Launch terminal application (not mutually exclusive with windows)
  - Attach OpenGL context
