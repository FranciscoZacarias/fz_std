
# (FZ) Personal standard library

Basic structure:

```c
#define DEBUG 1
#include "fz_base.h" // Required
#include "fz_window.h" // Launches window (requires base)
#include "fz_opengl.h" // Attaches opengl context to window (requires window)

function void
input_update()
{
  if (input_is_key_pressed(Keyboard_Key_ESCAPE))
  {
    os_exit_process(0);
  }
}

function void
entry_point(Command_Line* command_line)
{
  Arena* arena = arena_alloc();

  os_console_init();
  os_window_init(400, 400, S("fz_std_tests"));
  os_opengl_init();
  os_window_open();

  float phase = 0.0f;
  const float speed = 0.05f;
  const float tau = PI*2;

  os_window_enable_vsync(true);
  while(os_is_application_running())
  {
    input_update();

    phase += speed;
    if (phase > tau) phase -= tau;
    float t = (1.0f + cosf(phase)) * 0.5f;
    float r = (1.0f + cosf(phase)) * 0.5f;
    float g = (1.0f + cosf(phase - tau/3)) * 0.5f;
    float b = (1.0f + cosf(phase - 2*tau/3)) * 0.5f;
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    os_swap_buffers();
  }
}

```