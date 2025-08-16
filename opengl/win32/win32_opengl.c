function b32
os_opengl_init(OS_Window* window)
{
  if (!_win32_load_wgl_functions())
  {
    emit_fatal(S("Failed to load wgl functions."));
  }

  // set pixel format for OpenGL context
  {
    int attrib[] =
    {
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
    if (!wglChoosePixelFormatARB(window->device_context, attrib, NULL, 1, &format, &formats) || formats == 0)
    {
      emit_fatal(S("OpenGL does not support required pixel format!"));
    }

    PIXELFORMATDESCRIPTOR desc =
    {
      sizeof(PIXELFORMATDESCRIPTOR),
      1,
      PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    // Flags
      PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
      32,                   // Colordepth of the framebuffer.
      0, 0, 0, 0, 0, 0,
      0,
      0,
      0,
      0, 0, 0, 0,
      24,                   // Number of bits for the depthbuffer
      8,                    // Number of bits for the stencilbuffer
      0,                    // Number of Aux buffers in the framebuffer.
      PFD_MAIN_PLANE,
      0,
      0,
      0,
      0
    };
    if(!DescribePixelFormat(window->device_context, format, sizeof(desc), &desc))
    {
      emit_error(S("Failed to describe OpenGL pixel format"));
    }
    
    if (!SetPixelFormat(window->device_context, format, &desc))
    {
      emit_fatal(S("Cannot set OpenGL selected pixel format!"));
    }
  }

  // create modern OpenGL context
  {
    int attrib[] = {
      WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
      WGL_CONTEXT_MINOR_VERSION_ARB, 5,
      WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,

#ifndef DEBUG
      // ask for debug context for non "Release" builds
      // this is so we can enable debug callback
      WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
#endif
      0,
    };

    window->rendering_context = wglCreateContextAttribsARB(window->device_context, NULL, attrib);
    if (!window->rendering_context)
    {
      emit_fatal(S("Cannot create modern OpenGL context! OpenGL version 4.5 not supported?"));
    }

    b32 ok = wglMakeCurrent(window->device_context, window->rendering_context);
    if (!ok)
    {
      win32_check_error();
      emit_fatal(S("Failed to make current OpenGL context"));
    }

    if (!_os_opengl_load_functions())
    {
      emit_fatal(S("Opengl failed to load functions"));
    }

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(&_os_opengl_debug_callback, NULL);
  }

  // Set viewport
  int width = window->dimensions.x;
  int height = window->dimensions.y;
  glViewport(0, 0, width, height);

  // Check for errors
  GLenum error = glGetError();
  if (error != GL_NO_ERROR)
  {
    printf("OpenGL error after init: 0x%x\n", error);
  }

  return true;
}

function void
os_opengl_end(OS_Window* window)
{
  wglMakeCurrent(NULL, NULL);
  wglDeleteContext(window->rendering_context);
}

function void
os_window_enable_vsync(b32 state)
{
  wglSwapIntervalEXT(state);
}

function b32
_win32_load_wgl_functions()
{
  // to get WGL functions we need valid GL context, so create dummy window for dummy GL context
  HWND dummy = CreateWindowExW(
    0, L"STATIC", L"DummyWindow", WS_OVERLAPPED,
    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
    NULL, NULL, NULL, NULL);
  if (!dummy)
  {
    printf("Failed to create dummy window");
    return false;
  }

  HDC device_context = GetDC(dummy);
  if (!device_context)
  {
    printf("Failed to get device context for dummy window");
    return false;
  }

  PIXELFORMATDESCRIPTOR desc = {
    .nSize      = sizeof(desc),
    .nVersion   = 1,
    .dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
    .iPixelType = PFD_TYPE_RGBA,
    .cColorBits = 24,
  };

  int format = ChoosePixelFormat(device_context, &desc);
  if (!format) 
  {
    printf("Cannot choose OpenGL pixel format for dummy window!");
    return false;
  }

  if (!DescribePixelFormat(device_context, format, sizeof(desc), &desc))
  {
    printf("Failed to describe OpenGL pixel format");
    return false;
  }

  // reason to create dummy window is that SetPixelFormat can be called only once for the window
  if (!SetPixelFormat(device_context, format, &desc))
  {
    printf("Cannot set OpenGL pixel format for dummy window!");
    return false;
  }

  HGLRC rc = wglCreateContext(device_context);
  if (!rc)
  {
    printf("Failed to create OpenGL context for dummy window");
    return false;
  }

  if (!wglMakeCurrent(device_context, rc))
  {
    printf("Failed to make current OpenGL context for dummy window");
    return false;
  }

  {
    #define WGL_FUNC(ret, name, params) \
      name = (PFN##name##PROC)wglGetProcAddress(#name); \
      if (!name) { printf("%s not loaded", #name); return false; }
    # include "win32_wgl_functions.inl"
    #undef WGL_FUNC
  }

  wglMakeCurrent(NULL, NULL);
  wglDeleteContext(rc);
  ReleaseDC(dummy, device_context);
  DestroyWindow(dummy);

  return true;
}