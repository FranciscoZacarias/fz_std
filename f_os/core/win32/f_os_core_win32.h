#ifndef F_OS_WIN32
#define F_OS_WIN32

#include <windows.h>
#include <windowsx.h>
#include <timeapi.h>
#include <tlhelp32.h>
#include <Shlobj.h>

typedef struct OS_Win32_State {
  Arena* arena;

  HMODULE advapi_dll;
  SYSTEM_INFO system_info;
  LARGE_INTEGER counts_per_second;
  b32 granular_sleep_enabled;

} OS_Win32_State;

global HINSTANCE OSWin32HInstance;
global OS_Win32_State* OSWin32State;
BOOL (*RtlGenRandom)(VOID *RandomBuffer, ULONG RandomBufferLength);

#endif // F_OS_WIN32