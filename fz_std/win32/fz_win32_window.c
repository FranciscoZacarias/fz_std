LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
    case WM_SETCURSOR:
      if (LOWORD(lParam) == HTCLIENT) {
        win32_set_cursor(CURSOR_ARROW);
        return TRUE;
      } break;

    case WM_SIZE: {
      _win32_window_resize_callback(LOWORD(lParam), HIWORD(lParam));
      return 0;
    } break;

    // Keyboard keys
    case WM_KEYDOWN: {
      _input_process_keyboard_key((Keyboard_Key)wParam, TRUE);
      return 0;
    } break;
    case WM_KEYUP: {
      _input_process_keyboard_key((Keyboard_Key)wParam, FALSE);
      return 0;
    } break;

    // Mouse Cursor
    case WM_MOUSEMOVE: {
      if (_IgnoreNextMouseMove) {
        _IgnoreNextMouseMove = false;
        return 0;
      }
      s32 x = LOWORD(lParam);
      s32 y = HIWORD(lParam);
      _input_process_mouse_cursor((f32)x, (f32)y);
      return 0;
    } break;
    
    // Mouse Buttons
    case WM_LBUTTONDOWN: {
      _input_process_mouse_button(MouseButton_Left, TRUE);
      return 0;
    } break;
    case WM_LBUTTONUP: {
      _input_process_mouse_button(MouseButton_Left, FALSE);
      return 0;
    } break;
    case WM_RBUTTONDOWN: {
      _input_process_mouse_button(MouseButton_Right, TRUE);
      return 0;
    } break;
    case WM_RBUTTONUP: {
      _input_process_mouse_button(MouseButton_Right, FALSE);
      return 0;
    } break;
    case WM_MBUTTONDOWN: {
      _input_process_mouse_button(MouseButton_Middle, TRUE);
      return 0;
    } break;
    case WM_MBUTTONUP: {
      _input_process_mouse_button(MouseButton_Middle, FALSE);
      return 0;
    } break;

    case WM_DESTROY: {
      wglMakeCurrent(NULL, NULL);
      wglDeleteContext(_RenderingContextHandle);
      ReleaseDC(hWnd, _DeviceContextHandle);
      PostQuitMessage(0);
      return 0;
    } break;
  }
  return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  _hInstance = hInstance;
  entry_point();
  return _ApplicationReturn;
}

internal void _win32_window_resize_callback(s32 width, s32 height) {
  if (height == 0) height = 1;
  if (width == 0)  width = 1;
  WindowWidth  = width;
  WindowHeight = height;
  if (_IsOpenGLContextEnabled) {
    glViewport(0, 0, width, height);
  }
}

internal void win32_timer_init() {
  AssertNoReentry();
  QueryPerformanceFrequency(&_PerformanceFrequency);
}

internal void win32_timer_start(PerformanceTimer* timer) {
  QueryPerformanceCounter(&timer->start);
}

internal void win32_timer_end(PerformanceTimer* timer) {
  QueryPerformanceCounter(&timer->end);
  LONGLONG difference = timer->end.QuadPart - timer->start.QuadPart;
  timer->elapsed_seconds = (f32)difference / (f32)_PerformanceFrequency.QuadPart;
}

internal HWND _win32_window_create(HINSTANCE hInstance, s32 width, s32 height) {
  HWND result = {0};

  WNDCLASSEXW wc = {
    .cbSize        = sizeof(wc),
    .lpfnWndProc   = WndProc,
    .hInstance     = hInstance,
    .hIcon         = LoadIcon(NULL, IDI_APPLICATION),
    .hCursor       = LoadCursor(NULL, IDC_ARROW),
    .lpszClassName = L"opengl_window_class",
  };

  ATOM atom = RegisterClassExW(&wc);
  Assert(atom && "Failed to register window class");
    
  LPCSTR app_name = "f_program"; // TODO(fz): Function to rename window

  DWORD exstyle = WS_EX_APPWINDOW;
  DWORD style   = WS_OVERLAPPEDWINDOW;

  result = CreateWindowExW(
    exstyle, wc.lpszClassName, L"OpenGL Window", style,
    CW_USEDEFAULT, CW_USEDEFAULT, width, height,
    NULL, NULL, wc.hInstance, NULL);
  Assert(result && "Failed to create window");

  _DeviceContextHandle = GetDC(result);
  Assert(_DeviceContextHandle && "Failed to window device context");

  return result;
}

internal void win32_set_cursor(CursorType cursor) {
  HCURSOR hCursor = NULL;

  switch (cursor) {
    case CURSOR_ARROW: {
      hCursor = LoadCursor(NULL, IDC_ARROW);
      break;
    }
    case CURSOR_HAND: {
      hCursor = LoadCursor(NULL, IDC_HAND);
      break;
    }
    case CURSOR_CROSSHAIR: {
      hCursor = LoadCursor(NULL, IDC_CROSS);
      break;
    }
    case CURSOR_IBEAM: {
      hCursor = LoadCursor(NULL, IDC_IBEAM);
      break;
    }
    case CURSOR_WAIT: { 
      hCursor = LoadCursor(NULL, IDC_WAIT);
      break;
    }
    case CURSOR_SIZE_ALL: {
      hCursor = LoadCursor(NULL, IDC_SIZEALL);
      break;
    }
    default: {
      hCursor = LoadCursor(NULL, IDC_ARROW);
      break;
    }
  }

  if (hCursor) {
    SetCursor(hCursor);
  }
}

internal void win32_set_cursor_position(s32 x, s32 y) {
  SetCursorPos(x, y);
}

internal void win32_lock_cursor(b32 lock) {
  if (lock) {
    RECT rect;
    GetClientRect(_WindowHandle, &rect);
    POINT center = {(rect.right - rect.left) / 2, (rect.bottom - rect.top) / 2};
    ClientToScreen(_WindowHandle, &center);
    SetCursorPos(center.x, center.y);

    _IsCursorLocked      = true;
    _IgnoreNextMouseMove = true;

    // Reset deltas to avoid cursor jump
    _InputState.mouse_current.delta_x  = 0.0f;
    _InputState.mouse_current.delta_y  = 0.0f;
    _InputState.mouse_previous.delta_x = 0.0f;
    _InputState.mouse_previous.delta_y = 0.0f;
    MemoryCopyStruct(&_InputState.mouse_previous, &_InputState.mouse_current);
  } else {
    _IsCursorLocked = false;
  }
}

internal void win32_hide_cursor(b32 hide) {
  // Win32 quirk. It has an internal counter required to show the cursor.
  // The while loops just make sure it exhausts the counter and applies immediately.
  while (ShowCursor(hide ? FALSE : TRUE) >= 0 &&  hide);
  while (ShowCursor(hide ? FALSE : TRUE) < 0  && !hide);
}

internal void win32_put_pixel(s32 x, s32 y, COLORREF color) {
  if (_IsOpenGLContextEnabled) {
    ERROR_MESSAGE_AND_EXIT("Called win32_put_pixel with opengl context attached");
  }
  if (!_IsWindowEnabled) {
    ERROR_MESSAGE_AND_EXIT("Called win32_put_pixel win32_enable_window");
  }
  SetPixel(_DeviceContextHandle, x, y, color);
}

internal f32 win32_get_elapsed_time() {
  LARGE_INTEGER current;
  QueryPerformanceCounter(&current);
  LONGLONG ticks = current.QuadPart - _Timer_ElapsedTime.start.QuadPart;
  return (f32)ticks / (f32)_PerformanceFrequency.QuadPart;
}

internal void win32_fatal_error(const char* message, ...) {
  va_list argptr;
  va_start(argptr, message);
  vfprintf(stderr, message, argptr);
  va_end(argptr);
  MessageBoxA(NULL, message, "Error", MB_ICONEXCLAMATION);
  ExitProcess(0);
}


internal f32 win32_get_frame_time() {
  return _Timer_FrameTime.elapsed_seconds;
}

internal void win32_show_window(b32 show_window) {
  ShowWindow(_WindowHandle, show_window ? SW_SHOW : SW_HIDE);
  UpdateWindow(_WindowHandle);
}

internal void win32_init() {
  win32_timer_init();
  win32_timer_start(&_Timer_ElapsedTime);

  thread_context_init_and_attach(&MainThreadContext);
  win32_timer_start(&_Timer_FrameTime);
}

internal b32 win32_enable_console() {
  BOOL result = AllocConsole();
  if (!result) {
    return false;
  }
  FILE* fp;
  freopen_s(&fp, "CONOUT$", "w", stdout);
  freopen_s(&fp, "CONOUT$", "w", stderr);
  _IsTerminalEnabled = true;
  return true;
}

internal b32 win32_enable_window(s32 width, s32 height) {
  _input_init();

  _WindowHandle = _win32_window_create(_hInstance, width, height);
  if (!_WindowHandle) {
    ERROR_MESSAGE_AND_EXIT("Failed to get window handle\n");
    return false;
  }

  _DeviceContextHandle = GetDC(_WindowHandle);
  if (!_DeviceContextHandle) {
      ERROR_MESSAGE_AND_EXIT("Failed to get device context\n");
      return false;
  }

  _IsWindowEnabled = true;
  return true;
}

internal void win32_application_stop() {
  IsApplicationRunning = false;
}

internal b32  win32_application_is_running() {
  win32_timer_end(&_Timer_FrameTime);
  win32_timer_start(&_Timer_FrameTime);

  MSG msg = {0};
  if (_IsWindowEnabled) {
    _input_update();
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      if (msg.message == WM_QUIT)  IsApplicationRunning = false;
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
  if (!IsApplicationRunning) {
    _ApplicationReturn = (s32)msg.wParam;
    return false;
  }

  return true;
}


/////////////////////////////
//~ Opengl

#include "glcorearb.h"
#include "wglext.h"

internal int StringsAreEqual(const char* src, const char* dst, size_t dstlen)
{
    while (*src && dstlen-- && *dst)
    {
        if (*src++ != *dst++)
        {
            return 0;
        }
    }

    return (dstlen && *src == *dst) || (!dstlen && *src == 0);
}

internal PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = NULL;
internal PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
internal PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;

internal void win32_get_webgl_functions() {
  // to get WGL functions we need valid GL context, so create dummy window for dummy GL context
  HWND dummy = CreateWindowExW(
    0, L"STATIC", L"DummyWindow", WS_OVERLAPPED,
    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
    NULL, NULL, NULL, NULL);
  Assert(dummy && "Failed to create dummy window");

  HDC dc = GetDC(dummy);
  Assert(dc && "Failed to get device context for dummy window");

  PIXELFORMATDESCRIPTOR desc = {
    .nSize = sizeof(desc),
    .nVersion = 1,
    .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
    .iPixelType = PFD_TYPE_RGBA,
    .cColorBits = 24,
  };

  int format = ChoosePixelFormat(dc, &desc);
  if (!format) {
    win32_fatal_error("Cannot choose OpenGL pixel format for dummy window!");
  }

  int ok = DescribePixelFormat(dc, format, sizeof(desc), &desc);
  Assert(ok && "Failed to describe OpenGL pixel format");

  // reason to create dummy window is that SetPixelFormat can be called only once for the window
  if (!SetPixelFormat(dc, format, &desc)) {
    win32_fatal_error("Cannot set OpenGL pixel format for dummy window!");
  }

  HGLRC rc = wglCreateContext(dc);
  Assert(rc && "Failed to create OpenGL context for dummy window");

  ok = wglMakeCurrent(dc, rc);
  Assert(ok && "Failed to make current OpenGL context for dummy window");

  // https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_extensions_string.txt
  PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB = (void*)wglGetProcAddress("wglGetExtensionsStringARB");
  if (!wglGetExtensionsStringARB) {
    win32_fatal_error("OpenGL does not support WGL_ARB_extensions_string extension!");
  }

  const char* ext = wglGetExtensionsStringARB(dc);
  Assert(ext && "Failed to get OpenGL WGL extension string");

  const char* start = ext;
  for (;;)
  {
      while (*ext != 0 && *ext != ' ')
      {
          ext++;
      }

      size_t length = ext - start;
      if (StringsAreEqual("WGL_ARB_pixel_format", start, length))
      {
          // https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_pixel_format.txt
          wglChoosePixelFormatARB = (void*)wglGetProcAddress("wglChoosePixelFormatARB");
      }
      else if (StringsAreEqual("WGL_ARB_create_context", start, length))
      {
          // https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_create_context.txt
          wglCreateContextAttribsARB = (void*)wglGetProcAddress("wglCreateContextAttribsARB");
      }
      else if (StringsAreEqual("WGL_EXT_swap_control", start, length))
      {
          // https://www.khronos.org/registry/OpenGL/extensions/EXT/WGL_EXT_swap_control.txt
          wglSwapIntervalEXT = (void*)wglGetProcAddress("wglSwapIntervalEXT");
      }

      if (*ext == 0)
      {
          break;
      }

      ext++;
      start = ext;
  }

  if (!wglChoosePixelFormatARB || !wglCreateContextAttribsARB || !wglSwapIntervalEXT) {
      win32_fatal_error("OpenGL does not support required WGL extensions for modern context!");
  }

  wglMakeCurrent(NULL, NULL);
  wglDeleteContext(rc);
  ReleaseDC(dummy, dc);
  DestroyWindow(dummy);
}

// TODO(fz) if opengl is enabled...
internal void APIENTRY opengl_debug_callback(
    GLenum source, GLenum type, GLuint id, GLenum severity,
    GLsizei length, const GLchar* message, const void* user)
{
    OutputDebugStringA(message);
    OutputDebugStringA("\n");
    if (severity == GL_DEBUG_SEVERITY_HIGH || severity == GL_DEBUG_SEVERITY_MEDIUM)
    {
        if (IsDebuggerPresent())
        {
            Assert(!"OpenGL error - check the callstack in debugger");
        }
        win32_fatal_error("OpenGL API usage error! Use debugger to examine call stack!");
    }
}

// DOC(fz): https://gist.github.com/mmozeiko/ed2ad27f75edf9c26053ce332a1f6647
internal b32 win32_enable_opengl() {
  b32 result = true;

  win32_get_webgl_functions();

  // set pixel format for OpenGL context
  {
    int attrib[] = {
      WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
      WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
      WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
      WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
      WGL_COLOR_BITS_ARB,     24,
      WGL_DEPTH_BITS_ARB,     24,
      WGL_STENCIL_BITS_ARB,   8,

      // uncomment for sRGB framebuffer, from WGL_ARB_framebuffer_sRGB extension
      // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_framebuffer_sRGB.txt
      //WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, GL_TRUE,

      // uncomment for multisampled framebuffer, from WGL_ARB_multisample extension
      // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_multisample.txt
      //WGL_SAMPLE_BUFFERS_ARB, 1,
      //WGL_SAMPLES_ARB,        4, // 4x MSAA

      0,
    };

    int format;
    UINT formats;
    if (!wglChoosePixelFormatARB(_DeviceContextHandle, attrib, NULL, 1, &format, &formats) || formats == 0)
    {
        win32_fatal_error("OpenGL does not support required pixel format!");
    }

    PIXELFORMATDESCRIPTOR desc = { .nSize = sizeof(desc) };
    int ok = DescribePixelFormat(_DeviceContextHandle, format, sizeof(desc), &desc);
    Assert(ok && "Failed to describe OpenGL pixel format");

    if (!SetPixelFormat(_DeviceContextHandle, format, &desc))
    {
      win32_fatal_error("Cannot set OpenGL selected pixel format!");
    }
  }

  // create modern OpenGL context
  {
    int attrib[] = {
      WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
      WGL_CONTEXT_MINOR_VERSION_ARB, 6,
      WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,

      // ask for debug context for non "Release" builds
      // this is so we can enable debug callback
      WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,

      0,
    };

    HGLRC rc = wglCreateContextAttribsARB(_DeviceContextHandle, NULL, attrib);
    if (!rc) {
      win32_fatal_error("Cannot create modern OpenGL context! OpenGL version 4.5 not supported?");
    }

    result = wglMakeCurrent(_DeviceContextHandle, rc);
    Assert(result && "Failed to make current OpenGL context");

    if (!gladLoadGL()) {
      win32_fatal_error("Failed to load OpenGL functions with glad!");
    }

    // enable debug callback
    glDebugMessageCallback(&opengl_debug_callback, NULL);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  }

  return result;
}