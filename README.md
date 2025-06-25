
# (FZ) Personal standard library

Basic structure:

```c
#include "fz_include.h"

internal void entry_point() {
  win32_init();
  win32_enable_window();
  win32_enable_opengl();
  win32_show_window(true);

  while (win32_application_is_running()) {

    // ...

  }
}
```