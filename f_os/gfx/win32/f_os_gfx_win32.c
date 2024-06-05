#include <uxtheme.h>
#undef DeleteFile
#undef IsMaximized
#include <dwmapi.h>
#pragma comment(lib, "gdi32")
#pragma comment(lib, "dwmapi")
#pragma comment(lib, "UxTheme")
#pragma comment(lib, "ole32")

#define OS_W32_GraphicalWindowClassName L"ApplicationWindowClass"

per_thread Arena* OSWin32TLEventsArena;
per_thread OS_EventList *OSWin32TLEventsList;


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
  LRESULT result  = 0;
  OS_Event* event = 0;
  OS_Win32_Window* window = (OS_Win32_Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
  OS_Handle window_handle = os_win32_handle_from_window(window);
  Arena_Temp scratch = ScratchBegin(&OSWin32TLEventsArena, 1);
  OS_EventList fallback_event_list = {0};
  if(OSWin32TLEventsArena == 0) {
    OSWin32TLEventsArena = scratch.arena;
    OSWin32TLEventsList = &fallback_event_list;
  }
  b32 is_release = 0;
  Axis2 scroll_axis = Axis2_Y;
  switch(message) {
    default: {
      result = DefWindowProcW(hwnd, message, w_param, l_param);
    } break;

    //~ General Events
    case WM_CLOSE: {
      event = PushArray(OSWin32TLEventsArena, OS_Event, 1);
      event->kind = OS_EventKind_WindowClose;
      event->window = window_handle;
    } break;
    case WM_KILLFOCUS: {
      event = PushArray(OSWin32TLEventsArena, OS_Event, 1);
      event->kind = OS_EventKind_WindowLoseFocus;
      event->window = window_handle;
      ReleaseCapture();
    } break;

    //~ Mouse Events
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP: {
      ReleaseCapture();
      is_release = 1;
    }fallthrough;
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN: {
      if(is_release == 0) {
        SetCapture(hwnd);
      }
      OS_EventKind kind = is_release ? OS_EventKind_Release : OS_EventKind_Press;
      OS_Key key = OS_Key_MouseLeft;
      switch(message) {
        case WM_MBUTTONUP: case WM_MBUTTONDOWN: key = OS_Key_MouseMiddle; break;
        case WM_RBUTTONUP: case WM_RBUTTONDOWN: key = OS_Key_MouseRight; break;
      }
      event           = PushArray(OSWin32TLEventsArena, OS_Event, 1);
      event->kind     = kind;
      event->window   = window_handle;
      event->key      = key;
      event->position = os_mouse_from_window(window_handle);
    } break;

    //~ Mouse wheel
    case WM_MOUSEHWHEEL: scroll_axis = Axis2_X; goto scroll;
    case WM_MOUSEWHEEL:
    scroll:; {
      s16 wheel_delta = HIWORD(w_param);
      event = PushArray(OSWin32TLEventsArena, OS_Event, 1);
      event->kind = OS_EventKind_MouseScroll;
      event->window = window_handle;
      event->scroll.data[scroll_axis] = -(f32)wheel_delta;
    } break;

    //~ Cursor setting
    // TODO(fz): ...

    //~ Keyboard events
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP: {
      DefWindowProcW(hwnd, message, w_param, l_param);
    }fallthrough;
    case WM_KEYDOWN:
    case WM_KEYUP:
    {
      b32 was_down = !!(l_param & (1 << 30));
      b32 is_down =   !(l_param & (1 << 31));
      OS_EventKind kind = is_down ? OS_EventKind_Press : OS_EventKind_Release;
      
      local_persist OS_Key key_table[256] = {0};
      local_persist b32 key_table_initialized = 0;
      if(!key_table_initialized) {
        key_table_initialized = 1;
        
        for (u32 i = 'A', j = OS_Key_A; i <= 'Z'; i += 1, j += 1) {
          key_table[i] = (OS_Key)j;
        }
        for (u32 i = '0', j = OS_Key_0; i <= '9'; i += 1, j += 1) {
          key_table[i] = (OS_Key)j;
        }
        for (u32 i = VK_F1, j = OS_Key_F1; i <= VK_F24; i += 1, j += 1) {
          key_table[i] = (OS_Key)j;
        }
        
        key_table[VK_ESCAPE]        = OS_Key_Esc;
        key_table[VK_OEM_3]         = OS_Key_GraveAccent;
        key_table[VK_OEM_MINUS]     = OS_Key_Minus;
        key_table[VK_OEM_PLUS]      = OS_Key_Equal;
        key_table[VK_BACK]          = OS_Key_Backspace;
        key_table[VK_TAB]           = OS_Key_Tab;
        key_table[VK_SPACE]         = OS_Key_Space;
        key_table[VK_RETURN]        = OS_Key_Enter;
        key_table[VK_CONTROL]       = OS_Key_Ctrl;
        key_table[VK_SHIFT]         = OS_Key_Shift;
        key_table[VK_MENU]          = OS_Key_Alt;
        key_table[VK_UP]            = OS_Key_Up;
        key_table[VK_LEFT]          = OS_Key_Left;
        key_table[VK_DOWN]          = OS_Key_Down;
        key_table[VK_RIGHT]         = OS_Key_Right;
        key_table[VK_DELETE]        = OS_Key_Delete;
        key_table[VK_PRIOR]         = OS_Key_PageUp;
        key_table[VK_NEXT]          = OS_Key_PageDown;
        key_table[VK_HOME]          = OS_Key_Home;
        key_table[VK_END]           = OS_Key_End;
        key_table[VK_OEM_2]         = OS_Key_ForwardSlash;
        key_table[VK_OEM_PERIOD]    = OS_Key_Period;
        key_table[VK_OEM_COMMA]     = OS_Key_Comma;
        key_table[VK_OEM_7]         = OS_Key_Quote;
        key_table[VK_OEM_4]         = OS_Key_LeftBracket;
        key_table[VK_OEM_6]         = OS_Key_RightBracket;
        key_table[VK_INSERT]        = OS_Key_Insert;
        key_table[VK_OEM_1]         = OS_Key_Semicolon;
      }
      
      OS_Key key = OS_Key_Null;
      if(w_param < ArrayCount(key_table)) {
        key = key_table[w_param];
      }
      
      event = PushArray(OSWin32TLEventsArena, OS_Event, 1);
      event->kind = kind;
      event->window = window_handle;
      event->key = key;
    }break;
  }

  if(event) {
    event->modifiers = os_win32_get_modifiers();
    DLLPushBack(OSWin32TLEventsList->first, OSWin32TLEventsList->last, event);
    OSWin32TLEventsList->count += 1;
  }

  ScratchEnd(scratch);
  return result;
}

internal OS_Handle
os_win32_handle_from_window(OS_Win32_Window *window) {
  OS_Handle handle = {0};
  handle.u64[0] = (u64)window;
  return handle;
}

internal OS_Win32_Window*
os_win32_window_from_handle(OS_Handle handle) {
  OS_Win32_Window *w = (OS_Win32_Window *)handle.u64[0];
  return w;
}

internal OS_Modifiers
os_win32_get_modifiers() {
  OS_Modifiers modifiers = 0;
  if(GetKeyState(VK_CONTROL) & 0x8000) {
    modifiers |= OS_Modifier_Ctrl;
  }
  if(GetKeyState(VK_SHIFT) & 0x8000) {
    modifiers |= OS_Modifier_Shift;
  }
  if(GetKeyState(VK_MENU) & 0x8000) {
    modifiers |= OS_Modifier_Alt;
  }
  return modifiers;
}

internal Vec2f32
os_mouse_from_window(OS_Handle handle) {
  Vec2f32 result = vec2f32(-100, -100);
  OS_Win32_Window *window = os_win32_window_from_handle(handle);
  if(window != 0) {
    POINT point;
    if(GetCursorPos(&point)) {
      if(ScreenToClient(window->hwnd, &point)) {
        result = vec2f32((f32)point.x, (f32)point.y);
      }
    }
  }
  return result;
}