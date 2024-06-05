#include <uxtheme.h>
#undef DeleteFile
#undef IsMaximized
#include <dwmapi.h>
#pragma comment(lib, "gdi32")
#pragma comment(lib, "dwmapi")
#pragma comment(lib, "UxTheme")
#pragma comment(lib, "ole32")

#define OS_W32_GraphicalWindowClassName L"ApplicationWindowClass"

internal void
os_init_gfx() {
  if(is_main_thread() && OSWin32GfxState == 0) {

    //~ make global state
    {
      Arena *arena = arena_alloc(Gigabytes(1));
      OSWin32GfxState = PushArray(arena, OS_Win32_GfxState, 1);
      OSWin32GfxState->arena = arena;
      OSWin32GfxState->window_arena = arena_alloc(Gigabytes(1));
      InitializeSRWLock(&OSWin32GfxState->window_srw_lock);
    }

    //~ set dpi awareness
    OS_W32_SetProcessDpiAwarenessContextFunctionType *set_dpi_awareness_function = 0;
    HMODULE module = LoadLibraryA("user32.dll");
    if(module != 0) {
      set_dpi_awareness_function =
      (OS_W32_SetProcessDpiAwarenessContextFunctionType *)GetProcAddress(module, "SetProcessDpiAwarenessContext");
      FreeLibrary(module);
    }
    if(set_dpi_awareness_function != 0) {
      set_dpi_awareness_function(OS_W32_DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    }

    //~ load DPI fallback
    {
      HMODULE user32 = LoadLibraryA("user32.dll");
      Win32GetDpiForWindow = (OS_W32_GetDpiForWindowType*)GetProcAddress(user32, "GetDpiForWindow");
      FreeLibrary(user32);
    }

    //~ register window class
    {
      WNDCLASSW window_class = {0};
      window_class.style = CS_HREDRAW | CS_VREDRAW;
      window_class.lpfnWndProc = os_win32_window_proc;
      window_class.hInstance = OSWin32HInstance;
      window_class.lpszClassName = OS_W32_GraphicalWindowClassName;
      window_class.hCursor = LoadCursor(0, IDC_ARROW);
      RegisterClassW(&window_class);
    }

    //~ make global invisible window
    {
      OSWin32GfxState->global_hwnd = CreateWindowW(OS_W32_GraphicalWindowClassName,
                                                    L"",
                                                    WS_OVERLAPPEDWINDOW,
                                                    CW_USEDEFAULT, CW_USEDEFAULT,
                                                    100, 100,
                                                    0, 0,
                                                    OSWin32HInstance, 0);
      OSWin32GfxState->global_hdc = GetDC(OSWin32GfxState->global_hwnd);
    }


    //~ find refresh rate
    {
      OSWin32GfxState->refresh_rate = 60.f;
      DEVMODEA device_mode = {0};
      if(EnumDisplaySettingsA(0, ENUM_CURRENT_SETTINGS, &device_mode)) {
        OSWin32GfxState->refresh_rate = (f32)device_mode.dmDisplayFrequency;
      }
    }
  }
}

internal LRESULT
os_win32_window_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) {

}