#ifndef OS_CORE_WIN32_H
#define OS_CORE_WIN32_H

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "advapi32")

///////////////////////////////////////////////////////
// @Section: Error handling
function LONG WINAPI win32_exception_filter(EXCEPTION_POINTERS* exception_ptrs); /* Win32 exception filer for SetUnhandledExceptionFilter */
function void _win32_output_last_error(DWORD error);
#define win32_check_error() Statement(DWORD e = GetLastError(); if (e != 0) { _win32_output_last_error(e); })

///////////////////////////////////////////////////////
// @Section: Timer
global LARGE_INTEGER g_win32_performance_frequency;

///////////////////////////////////////////////////////
// @Section: Modern Windows SDK functions
// rfj: We must dynamically link to them, since they can be missing in older SDKs
typedef HRESULT W32_SetThreadDescription_Type(HANDLE hThread, PCWSTR lpThreadDescription);
global W32_SetThreadDescription_Type *g_win32_set_thread_description_func = 0;

///////////////////////////////////////////////////////
// @Section: Extern
extern int    __argc;
extern char** __argv;

///////////////////////////////////////////////////////
// @Section: Win32 Globals
global HINSTANCE _g_hInstance          = NULL;
global WPARAM    _g_application_return = 0;

#endif // OS_CORE_WIN32_H