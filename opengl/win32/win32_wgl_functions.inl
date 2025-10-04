// WGL extensions
WGL_FUNC(BOOL,  wglChoosePixelFormatARB,    (HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats))
WGL_FUNC(HGLRC, wglCreateContextAttribsARB, (HDC hdc, HGLRC hShareContext, const int *attribList))

// Vsync control
WGL_FUNC(BOOL, wglSwapIntervalEXT,    (int interval))
WGL_FUNC(int,  wglGetSwapIntervalEXT, ())