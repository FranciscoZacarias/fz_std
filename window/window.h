#ifndef OS_WINDOW_H
#define OS_WINDOW_H

/* fz_std only supports one window. */

///////////////////////////////////////////////////////
// @Section: Window
typedef struct OS_Window OS_Window;
struct OS_Window
{
  Vec2S32 dimensions;
  String8 title;
};
global OS_Window g_os_window;

///////////////////////////////////////////////////////
// @Section: Window Lifecycle
function b32       os_window_init(s32 width, s32 height, String8 title); /* Creates a window */
function void      os_window_open();                                     /* Shows a window */
function void      os_window_close();                                    /* Closes and destroys a window */
function b32       os_is_application_running();                             /* Swaps buffers for the only existing window */
function OS_Window os_window_get();                                      /* Returns the window handle */

///////////////////////////////////////////////////////
// @Section: Window Flags
function b32  os_window_is_fullscreen();         /* True if fullscreen */
function void os_window_set_fullscreen(b32 set); /* Enables/disables fullscreen */
function b32  os_window_is_maximized();          /* True if maximized */
function void os_window_set_maximized(b32 set);  /* Maximizes/restores window */
function b32  os_window_is_minimized();          /* True if minimized */
function void os_window_set_minimized(b32 set);  /* Minimizes/restores window */
function void os_swap_buffers();                 /* Swaps buffers */

///////////////////////////////////////////////////////
// @Section: Window Appearance
function void os_window_set_visible(b32 visible);                          /* Show or hide the window */
function b32  os_window_set_title(String8 title);                          /* Sets window title */
function void os_window_clear_custom_border_data();                        /* Resets border override (Windows only) */
function void os_window_push_custom_title_bar(f32 thickness);              /* Define title bar area */
function void os_window_push_custom_edges(f32 thickness);                  /* Define draggable edges */
function void os_window_push_custom_title_bar_client_area(Range2F32 rect); /* Client title bar rect */
function void os_window_set_position(Vec2F32 pos);                         /* Set window top-left position */
function void os_window_set_size(s32 width, s32 height);                   /* Set client size (non-fullscreen) */

#endif // OS_WINDOW_H
