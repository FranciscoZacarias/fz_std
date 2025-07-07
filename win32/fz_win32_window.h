#ifndef FZ_WIN32_PLATFORM_H
#define FZ_WIN32_PLATFORM_H

// DOC(fz): User space API is at the bottom of the file. Look for the string //~ APPLICATION SPACE

///////////////////////
//~ Timer
typedef struct PerformanceTimer {
  LARGE_INTEGER start;
  LARGE_INTEGER end;
  f32 elapsed_seconds;
} PerformanceTimer;

global LARGE_INTEGER    _PerformanceFrequency;
global PerformanceTimer _Timer_FrameTime   = {0};
global PerformanceTimer _Timer_ElapsedTime = {0};

///////////////////////
//~ Window

global HGLRC _RenderingContextHandle = NULL;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
internal HWND _win32_window_create(HINSTANCE hInstance, s32 width, s32 height);
internal void _win32_window_resize_callback(s32 width, s32 height);

///////////////////////
//~ Win32

global HDC       _DeviceContextHandle  = NULL;
global HWND      _WindowHandle         = NULL;
global WPARAM    _ApplicationReturn    = 0;
global HINSTANCE _hInstance            = (void*)0;

global b32    _IsWindowEnabled         = false;
global b32    _IsOpenGLContextEnabled  = false;
global b32    _IsTerminalEnabled       = false;

///////////////////////
//~ Cursor

typedef enum Cursor_Type {
  CURSOR_ARROW,
  CURSOR_HAND,
  CURSOR_CROSSHAIR,
  CURSOR_IBEAM,
  CURSOR_WAIT,
  CURSOR_SIZE_ALL,

  CURSOR_COUNT
} CursorType;

global b32 _IgnoreNextMouseMove = false;
global b32 _IsCursorLocked      = false;

/////////////////////////
//~ APPLICATION SPACE 
//~ Doc(fz): User can access directly anything declared here. Variables, functions...

global b32 IsApplicationRunning = true;
global s32 WindowWidth  = 0.0f;
global s32 WindowHeight = 0.0f;
internal void entry_point(); // DOC(fz): Application layer must implement this function as it's entry point.

// Window
internal void win32_init();
internal b32  win32_enable_console();
internal b32  win32_enable_window(s32 width, s32 height);
internal b32  win32_enable_opengl();
internal void win32_get_webgl_functions(); 

internal void win32_show_window(b32 show_window);
internal b32  win32_application_is_running();
internal void win32_application_stop();
internal void win32_put_pixel(s32 x, s32 y, COLORREF color); // DOC(fz): Colors a specific pixel. Requires win32_enable_window and must not have opengl context attached.

// Cursor
internal void win32_set_cursor(CursorType cursor);
internal void win32_set_cursor_position(s32 x, s32 y);
internal void win32_lock_cursor(b32 lock);
internal void win32_hide_cursor(b32 hide);

// Timer
internal void win32_timer_init();
internal void win32_timer_start(PerformanceTimer* timer);
internal void win32_timer_end(PerformanceTimer* timer);

// Helpers
internal f32  win32_get_elapsed_time();
internal void win32_fatal_error(const char* message, ...);

//~ END APPLICATION SPACE
/////////////////////////

#endif // FZ_WIN32_PLATFORM_H














