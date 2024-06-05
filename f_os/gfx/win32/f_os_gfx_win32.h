#ifndef OS_GFX_WIN32_H
#define OS_GFX_WIN32_H

typedef enum OS_CursorKind {
  OS_CursorKind_Null,
  OS_CursorKind_Hidden,
  OS_CursorKind_Pointer,
  OS_CursorKind_Hand,
  OS_CursorKind_WestEast,
  OS_CursorKind_NorthSouth,
  OS_CursorKind_NorthEastSouthWest,
  OS_CursorKind_NorthWestSouthEast,
  OS_CursorKind_AllCardinalDirections,
  OS_CursorKind_IBar,
  OS_CursorKind_Blocked,
  OS_CursorKind_Loading,
  OS_CursorKind_Pan,

  OS_CursorKind_COUNT
} OS_CursorKind;

typedef UINT OS_W32_GetDpiForWindowType(HWND hwnd);
#define OS_W32_DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((void *)-4)
typedef BOOL OS_W32_SetProcessDpiAwarenessContextFunctionType(void *value);

typedef struct OS_Win32_Window OS_Win32_Window;
struct OS_Win32_Window {
  OS_Win32_Window *next;
  OS_Win32_Window *prev;
  HWND hwnd;
  HDC hdc;
  b32 last_window_placement_initialized;
  WINDOWPLACEMENT last_window_placement;
};

typedef struct OS_Win32_GfxState {
  Arena *arena;

  f32 refresh_rate;

  HWND global_hwnd;
  HDC global_hdc;

  OS_CursorKind cursor_kind;

  Arena* window_arena;
  SRWLOCK window_srw_lock;
  OS_Win32_Window *first_window;
  OS_Win32_Window *last_window;
  OS_Win32_Window *free_window;
} OS_Win32_GfxState;

global OS_W32_GetDpiForWindowType* Win32GetDpiForWindow;
global OS_Win32_GfxState* OSWin32GfxState;;

//~ Helpers

internal OS_Handle        os_win32_handle_from_window(OS_Win32_Window *window);
internal OS_Win32_Window* os_win32_window_from_handle(OS_Handle handle);
internal OS_Modifiers     os_win32_get_modifiers();

//~ Window Proc

internal LRESULT os_win32_window_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param);

#endif // OS_GFX_WIN32_H