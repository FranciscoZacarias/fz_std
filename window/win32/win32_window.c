///////////////////////////////////////////////////////
// @Section: Cursor
function void
os_cursor_set(Cursor_Type cursor)
{
  HCURSOR hCursor = NULL;

  switch (cursor)
  {
    case CURSOR_ARROW:
    {
      hCursor = LoadCursor(NULL, IDC_ARROW);
    }
    break;
    case CURSOR_HAND:
    {
      hCursor = LoadCursor(NULL, IDC_HAND);
    }
    break;
    case CURSOR_CROSSHAIR:
    {
      hCursor = LoadCursor(NULL, IDC_CROSS);
    }
    break;
    case CURSOR_IBEAM:
    {
      hCursor = LoadCursor(NULL, IDC_IBEAM);
    }
    break;
    case CURSOR_WAIT:
    { 
      hCursor = LoadCursor(NULL, IDC_WAIT);
    }
    break;
    case CURSOR_SIZE_ALL:
    {
      hCursor = LoadCursor(NULL, IDC_SIZEALL);
    }
    break;
    default:
    {
      hCursor = LoadCursor(NULL, IDC_ARROW);
    }
    break;
  }

  if (hCursor)
  {
    SetCursor(hCursor);
  }
}

function void
os_cursor_set_position(s32 x, s32 y)
{
  SetCursorPos(x, y);
}

function void
os_cursor_lock(b32 lock)
{
  if (lock)
  {
    RECT rect;
    GetClientRect(g_os_window_win32.window_handle, &rect);
    POINT center = {(rect.right - rect.left) / 2, (rect.bottom - rect.top) / 2};
    ClientToScreen(g_os_window_win32.window_handle, &center);
    SetCursorPos(center.x, center.y);

    _g_ignore_next_mouse_move = true;
    _g_is_cursor_locked       = true;

    // Reset deltas to avoid cursor jump
    _g_input_state.mouse_current.delta.x  = 0.0f;
    _g_input_state.mouse_current.delta.y  = 0.0f;
    _g_input_state.mouse_previous.delta.x = 0.0f;
    _g_input_state.mouse_previous.delta.y = 0.0f;
    MemoryCopyStruct(&_g_input_state.mouse_previous, &_g_input_state.mouse_current);
  }
  else
  {
    _g_is_cursor_locked = false;
  }
}

function void
os_cursor_hide(b32 hide)
{
  // Win32 quirk. It has an internal counter required to show the cursor.
  // The while loops just make sure it exhausts the counter and applies immediately.
  while (ShowCursor(hide ? FALSE : TRUE) >= 0 &&  hide);
  while (ShowCursor(hide ? FALSE : TRUE) < 0  && !hide);
}

///////////////////////////////////////////////////////
// @Section: Input-Keyboard

// Win32 VK code translation table - MUST match enum order exactly
static u32 _win32_key_table[Keyboard_Key_Count] =
{
  0x08, // Keyboard_Key_BACKSPACE
  0x0D, // Keyboard_Key_ENTER
  0x09, // Keyboard_Key_TAB
  0x10, // Keyboard_Key_SHIFT
  0x11, // Keyboard_Key_CONTROL
  0x13, // Keyboard_Key_PAUSE
  0x14, // Keyboard_Key_CAPS_LOCK
  0x1B, // Keyboard_Key_ESCAPE
  0x1C, // Keyboard_Key_CONVERT
  0x1D, // Keyboard_Key_NONCONVERT
  0x1E, // Keyboard_Key_ACCEPT
  0x1F, // Keyboard_Key_MODECHANGE
  0x20, // Keyboard_Key_SPACE
  0x21, // Keyboard_Key_PRIOR
  0x22, // Keyboard_Key_NEXT
  0x23, // Keyboard_Key_END
  0x24, // Keyboard_Key_HOME
  0x25, // Keyboard_Key_ARROW_LEFT
  0x26, // Keyboard_Key_ARROW_UP
  0x27, // Keyboard_Key_ARROW_RIGHT
  0x28, // Keyboard_Key_ARROW_DOWN
  0x29, // Keyboard_Key_SELECT
  0x2A, // Keyboard_Key_PRINT
  0x2B, // Keyboard_Key_EXECUTE
  0x2C, // Keyboard_Key_SNAPSHOT
  0x2D, // Keyboard_Key_INSERT
  0x2E, // Keyboard_Key_DELETE
  0x2F, // Keyboard_Key_HELP
  0x30, // Keyboard_Key_0
  0x31, // Keyboard_Key_1
  0x32, // Keyboard_Key_2
  0x33, // Keyboard_Key_3
  0x34, // Keyboard_Key_4
  0x35, // Keyboard_Key_5
  0x36, // Keyboard_Key_6
  0x37, // Keyboard_Key_7
  0x38, // Keyboard_Key_8
  0x39, // Keyboard_Key_9
  0x41, // Keyboard_Key_A
  0x42, // Keyboard_Key_B
  0x43, // Keyboard_Key_C
  0x44, // Keyboard_Key_D
  0x45, // Keyboard_Key_E
  0x46, // Keyboard_Key_F
  0x47, // Keyboard_Key_G
  0x48, // Keyboard_Key_H
  0x49, // Keyboard_Key_I
  0x4A, // Keyboard_Key_J
  0x4B, // Keyboard_Key_K
  0x4C, // Keyboard_Key_L
  0x4D, // Keyboard_Key_M
  0x4E, // Keyboard_Key_N
  0x4F, // Keyboard_Key_O
  0x50, // Keyboard_Key_P
  0x51, // Keyboard_Key_Q
  0x52, // Keyboard_Key_R
  0x53, // Keyboard_Key_S
  0x54, // Keyboard_Key_T
  0x55, // Keyboard_Key_U
  0x56, // Keyboard_Key_V
  0x57, // Keyboard_Key_W
  0x58, // Keyboard_Key_X
  0x59, // Keyboard_Key_Y
  0x5A, // Keyboard_Key_Z
  0x5B, // Keyboard_Key_LEFT_WIN
  0x5C, // Keyboard_Key_RIGHT_WIN
  0x5D, // Keyboard_Key_APPS
  0x5F, // Keyboard_Key_SLEEP
  0x60, // Keyboard_Key_NUMPAD0
  0x61, // Keyboard_Key_NUMPAD1
  0x62, // Keyboard_Key_NUMPAD2
  0x63, // Keyboard_Key_NUMPAD3
  0x64, // Keyboard_Key_NUMPAD4
  0x65, // Keyboard_Key_NUMPAD5
  0x66, // Keyboard_Key_NUMPAD6
  0x67, // Keyboard_Key_NUMPAD7
  0x68, // Keyboard_Key_NUMPAD8
  0x69, // Keyboard_Key_NUMPAD9
  0x6A, // Keyboard_Key_NUMPAD_MULTIPLY
  0x6B, // Keyboard_Key_NUMPAD_ADD
  0x6C, // Keyboard_Key_NUMPAD_SEPARATOR
  0x6D, // Keyboard_Key_NUMPAD_SUBTRACT
  0x6E, // Keyboard_Key_NUMPAD_DECIMAL
  0x6F, // Keyboard_Key_NUMPAD_DIVIDE
  0x70, // Keyboard_Key_F1
  0x71, // Keyboard_Key_F2
  0x72, // Keyboard_Key_F3
  0x73, // Keyboard_Key_F4
  0x74, // Keyboard_Key_F5
  0x75, // Keyboard_Key_F6
  0x76, // Keyboard_Key_F7
  0x77, // Keyboard_Key_F8
  0x78, // Keyboard_Key_F9
  0x79, // Keyboard_Key_F10
  0x7A, // Keyboard_Key_F11
  0x7B, // Keyboard_Key_F12
  0x7C, // Keyboard_Key_F13
  0x7D, // Keyboard_Key_F14
  0x7E, // Keyboard_Key_F15
  0x7F, // Keyboard_Key_F16
  0x80, // Keyboard_Key_F17
  0x81, // Keyboard_Key_F18
  0x82, // Keyboard_Key_F19
  0x83, // Keyboard_Key_F20
  0x84, // Keyboard_Key_F21
  0x85, // Keyboard_Key_F22
  0x86, // Keyboard_Key_F23
  0x87, // Keyboard_Key_F24
  0x90, // Keyboard_Key_NUMLOCK
  0x91, // Keyboard_Key_SCROLL
  0x92, // Keyboard_Key_NUMPAD_EQUAL
  0xA0, // Keyboard_Key_LEFT_SHIFT
  0xA1, // Keyboard_Key_RIGHT_SHIFT
  0xA2, // Keyboard_Key_LEFT_CONTROL
  0xA3, // Keyboard_Key_RIGHT_CONTROL
  0xA4, // Keyboard_Key_LEFT_MENU
  0xA5, // Keyboard_Key_RIGHT_MENU
  0xBA, // Keyboard_Key_SEMICOLON
  0xBB, // Keyboard_Key_PLUS
  0xBC, // Keyboard_Key_COMMA
  0xBD, // Keyboard_Key_MINUS
  0xBE, // Keyboard_Key_PERIOD
  0xBF, // Keyboard_Key_SLASH
  0xC0, // Keyboard_Key_GRAVE
  0xDB, // Keyboard_Key_LEFT_BRACKET
  0xDC, // Keyboard_Key_BACKSLASH
  0xDD, // Keyboard_Key_RIGHT_BRACKET
  0xDE, // Keyboard_Key_QUOTE
};

function u32 
_native_key_from_os_key(Keyboard_Key key)
{
  return _win32_key_table[key];
}

function Keyboard_Key 
_os_key_from_native_key(u32 native_key)
{
  for(u32 i = 0; i < Keyboard_Key_Count; ++i)
  {
    if(_win32_key_table[i] == native_key)
    {
      return (Keyboard_Key)i;
    }
  }
  return Keyboard_Key_Count; // invalid
}

///////////////////////////////////////////////////////
// @Section: Window Lifecycle
function b32
os_window_init(s32 width, s32 height, String8 title)
{
  MemoryZeroStruct(&g_os_window_win32);
  b32 result = true;

  g_os_window_win32.window_handle = _win32_window_create(_g_hInstance, width, height, title);
  if (!IsWindow(g_os_window_win32.window_handle))
  {
    win32_check_error();
    emit_error(S("Failed to get window handle\n"));
  }
  
  g_os_window_win32.device_context = GetDC(g_os_window_win32.window_handle);
  if (!g_os_window_win32.device_context)
  {
    win32_check_error();
    emit_error(S("Failed to get device context"));
  }

  g_os_window_win32.state.dimensions = (Vec2S32){width, height};
  g_os_window_win32.state.title      = S("FZ_Window_Title");
  
  _input_init();
  g_os_resize_callback = _win32_window_resize_callback;

  return result;
}

function void
os_window_open()
{
  ShowWindow(g_os_window_win32.window_handle, SW_SHOW);
  UpdateWindow(g_os_window_win32.window_handle);
}

function void     
os_window_close()
{
  ShowWindow(g_os_window_win32.window_handle, SW_HIDE);
  UpdateWindow(g_os_window_win32.window_handle);
}

function b32
os_is_application_running()
{
  b32 result = true;

  MSG msg = {0};
  if (g_os_window_win32.window_handle != NULL)
  {
    _input_update();

    _update_frame_info();

    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      if (msg.message == WM_QUIT) 
      {
        _g_application_return = (s32)msg.wParam;
        return false;
      }
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
  return result;
}

function OS_Window
os_window_get()
{
  return g_os_window;
}

///////////////////////////////////////////////////////
// @Section: Window Flags

function b32
os_window_is_fullscreen()
{
  RECT rect;
  GetWindowRect(g_os_window_win32.window_handle, &rect);

  MONITORINFO mi = { sizeof(mi) };
  GetMonitorInfo(MonitorFromWindow(g_os_window_win32.window_handle, MONITOR_DEFAULTTONEAREST), &mi);

  b32 result = (rect.left   == mi.rcMonitor.left &&
                rect.top    == mi.rcMonitor.top &&
                rect.right  == mi.rcMonitor.right &&
                rect.bottom == mi.rcMonitor.bottom);

  return result;
}

function void
os_window_set_fullscreen(b32 set)
{
  static WINDOWPLACEMENT prev = { sizeof(prev) };
  if (set)
  {
    GetWindowPlacement(g_os_window_win32.window_handle, &prev);

    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfo(MonitorFromWindow(g_os_window_win32.window_handle, MONITOR_DEFAULTTONEAREST), &mi);

    SetWindowLong(g_os_window_win32.window_handle, GWL_STYLE, WS_POPUP | WS_VISIBLE);
    SetWindowPos(g_os_window_win32.window_handle, HWND_TOP,
                 mi.rcMonitor.left, mi.rcMonitor.top,
                 mi.rcMonitor.right - mi.rcMonitor.left,
                 mi.rcMonitor.bottom - mi.rcMonitor.top,
                 SWP_FRAMECHANGED | SWP_NOOWNERZORDER);
  }
  else
  {
    SetWindowLong(g_os_window_win32.window_handle, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
    SetWindowPlacement(g_os_window_win32.window_handle, &prev);
    SetWindowPos(g_os_window_win32.window_handle, NULL, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                 SWP_FRAMECHANGED | SWP_NOOWNERZORDER);
  }
}

function b32
os_window_is_maximized()
{
  b32 result = IsZoomed(g_os_window_win32.window_handle);
  return result;
}

function void
os_window_set_maximized(b32 set)
{
  ShowWindow(g_os_window_win32.window_handle, set ? SW_MAXIMIZE : SW_RESTORE);
}

function b32
os_window_is_minimized()
{
  b32 result = IsIconic(g_os_window_win32.window_handle);
  return result;
}

function void
os_window_set_minimized(b32 set)
{
  ShowWindow(g_os_window_win32.window_handle, set ? SW_MINIMIZE : SW_RESTORE);
}

function void
os_swap_buffers()
{
  SwapBuffers(g_os_window_win32.device_context);
}

///////////////////////////////////////////////////////
// @Section: Window Appearance

function void
os_window_set_visible(b32 visible)
{
  ShowWindow(g_os_window_win32.window_handle, visible ? SW_SHOW : SW_HIDE);
}

function b32
os_window_set_title(String8 title)
{
  Scratch scratch = scratch_begin(0, 0);
  char* ctitle = cstring_from_string8(scratch.arena, title);
  b32 result = SetWindowTextA(g_os_window_win32.window_handle, ctitle);
  win32_check_error();
  // TODO(fz): Add title to g_os_window
  scratch_end(&scratch);
  return result;
}

function void
os_window_clear_custom_border_data()
{

}

function void
os_window_push_custom_title_bar(f32 thickness)
{

}

function void
os_window_push_custom_edges(f32 thickness)
{

}

function void
os_window_push_custom_title_bar_client_area(Range2F32 rect)
{

}

function void
os_window_set_position(Vec2F32 pos)
{
  SetWindowPos(g_os_window_win32.window_handle, 0,
               (int)pos.x, (int)pos.y, 0, 0,
               SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

function void
os_window_set_size(s32 width, s32 height)
{
  SetWindowPos(g_os_window_win32.window_handle, 0,
               0, 0, width, height,
               SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
}

///////////////////////////////////////////////////////
// @Section: Win32

LRESULT CALLBACK 
WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message) 
  {
    case WM_SETCURSOR:
    {
      if (LOWORD(lParam) == HTCLIENT) 
      {
        os_cursor_set(CURSOR_ARROW);
        return true;
      }
    }
    break;

    case WM_SIZE: 
    {
      g_os_resize_callback(LOWORD(lParam), HIWORD(lParam));
      return 0;
    }
    break;

    // Keyboard keys
    case WM_KEYDOWN: 
    {
      Keyboard_Key key = _os_key_from_native_key((u32)wParam);
      if (key < Keyboard_Key_Count) 
      {
        _input_process_keyboard_key((Keyboard_Key)key, true);
      }
      else
      {
        // TODO(fz): Handle error
      }
      return 0;
    }
    break;
    case WM_KEYUP: 
    {
      Keyboard_Key key = _os_key_from_native_key((u32)wParam);
      if (key < Keyboard_Key_Count) 
      {
        _input_process_keyboard_key((Keyboard_Key)key, false);
      }
      else
      {
        // TODO(fz): Handle error
      }
      return 0;
    }
    break;

    // Mouse Cursor
    case WM_MOUSEMOVE: 
    {
      if (_g_ignore_next_mouse_move) 
      {
        _g_ignore_next_mouse_move = false;
        return 0;
      }
      s32 x = LOWORD(lParam);
      s32 y = HIWORD(lParam);
      _input_process_mouse_cursor((f32)x, (f32)y);
      return 0;
    }
    break;
    
    // Mouse Buttons
    case WM_LBUTTONDOWN: 
    {
      _input_process_mouse_button(MouseButton_Left, true);
      return 0;
    }
    break;
    case WM_LBUTTONUP: 
    {
      _input_process_mouse_button(MouseButton_Left, false);
      return 0;
    }
    break;
    case WM_RBUTTONDOWN: 
    {
      _input_process_mouse_button(MouseButton_Right, true);
      return 0;
    }
    break;
    case WM_RBUTTONUP: 
    {
      _input_process_mouse_button(MouseButton_Right, false);
      return 0;
    }
    break;
    case WM_MBUTTONDOWN: 
    {
      _input_process_mouse_button(MouseButton_Middle, true);
      return 0;
    }
    break;
    case WM_MBUTTONUP: 
    {
      _input_process_mouse_button(MouseButton_Middle, false);
      return 0;
    }
    break;

    case WM_CLOSE:
    {
      DestroyWindow(hWnd);
      return 0;
    }
    break;

    case WM_DESTROY: 
    {
      ReleaseDC(hWnd, g_os_window_win32.device_context);
      PostQuitMessage(0);
      return 0;
    }
    break;
  }
  return DefWindowProc(hWnd, message, wParam, lParam);
}

function HWND
_win32_window_create(HINSTANCE hInstance, s32 width, s32 height, String8 title)
{
  WNDCLASSEXA wc = 
  {
    .cbSize        = sizeof(wc),
    .lpfnWndProc   = WndProc,
    .hInstance     = hInstance,
    .hIcon         = LoadIconA(NULL, MAKEINTRESOURCEA(IDI_APPLICATION)),
    .hCursor       = LoadCursorA(NULL, MAKEINTRESOURCEA(IDC_ARROW)),
    .lpszClassName = "FZ_Window_Class",
  };

  ATOM atom = RegisterClassExA(&wc);
  Assert(atom && "Failed to register window class");
    
  DWORD exstyle = WS_EX_APPWINDOW;
  DWORD style   = WS_OVERLAPPEDWINDOW;

  Scratch scratch = scratch_begin(0, 0);
  HWND result = CreateWindowExA(exstyle, wc.lpszClassName, cstring_from_string8(scratch.arena, title), style, CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, wc.hInstance, NULL);
  if (!result)
  {
    win32_check_error();
    emit_fatal(S("Error creating Win32 window."));
  }

  scratch_end(&scratch);
  return result;
}

function void
_win32_window_resize_callback(s32 width, s32 height)
{
  if (height == 0) height = 1;
  if (width == 0)  width  = 1;
  g_os_window_win32.state.dimensions.x = width;
  g_os_window_win32.state.dimensions.y = height;
  glViewport(0, 0, width, height);
}