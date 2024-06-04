#ifndef OS_GFX_WIN32_H
#define OS_GFX_WIN32_H

typedef struct OS_Win32_Window {
  HWND hwnd;
  HDC hdc;

} OS_Win32_Window;

#endif // OS_GFX_WIN32_H