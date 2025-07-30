///////////////////////////////////////////////////////
// @Section: System info
///////////////////////////////////////////////////////
// @Section: Process Info
///////////////////////////////////////////////////////
// @Section: Process Launch Parameters
///////////////////////////////////////////////////////
// @Section: Memory
function OS_Memory_Stats
os_memory_stats()
{
  OS_Memory_Stats result = {0};

  MEMORYSTATUSEX status = {0};
  status.dwLength = sizeof(status);

  if(GlobalMemoryStatusEx(&status))
  {
    result.total_physical     = (u64)status.ullTotalPhys;
    result.available_physical = (u64)status.ullAvailPhys;
    result.total_virtual      = (u64)status.ullTotalPageFile;
    result.used_virtual       = (u64)(status.ullTotalPageFile - status.ullAvailPageFile);
  }

  return result;
}

function void*
os_memory_reserve(u64 size)
{
  void *result = VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
  return result;
}

function b32
os_memory_commit(void *ptr, u64 size)
{
  b32 result = (VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != 0);
  return result;
}

function void
os_memory_decommit(void *ptr, u64 size)
{
  VirtualFree(ptr, size, MEM_DECOMMIT);
}

function void
os_memory_release(void *ptr, u64 size)
{
  // NOTE(fz): Size not needed for windows. Though required for other OS's.
  VirtualFree(ptr, 0, MEM_RELEASE);
}

function u64
os_memory_get_page_size()
{
  SYSTEM_INFO info;
  GetSystemInfo(&info);
  u64 result = (u64)info.dwPageSize;
  return result;
}

///////////////////////////////////////////////////////
// @Section: Console
function b32
os_console_init()
{
  b32 result = false;

  if (GetConsoleWindow() != NULL)
  {
    // Already attached to a console; no need to allocate a new one.
    result = false;
  }
  else if (AllocConsole())
  {
    // No console attached; allocate a new one.
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    result = true;
  }

  // Try to enable color
  HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
  if (handle != INVALID_HANDLE_VALUE)
  {
    DWORD mode = 0;
    if (GetConsoleMode(handle, &mode))
    {
      if ((mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) == 0)
      {
        SetConsoleMode(handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
      }
    }
  }

  return result;
}

function void
os_console_write(String8 string)
{
  printf("%.*s", (s32)string.size, string.str);
}

function void
os_console_write_line(String8 string)
{

}

function void
os_console_writef(char* fmt, ...)
{

}

function void
os_console_set_text_color(u8 r, u8 g, u8 b)
{

}

function void
os_console_reset_text_color()
{

}

function String8
os_console_read_line(Arena* arena)
{

}

function b32
os_console_has_input()
{

}

///////////////////////////////////////////////////////
// @Section: Files
function b32
os_file_create(String8 path)
{
  Scratch scratch = scratch_begin(0,0);
  u8* cpath = cstring_from_string8(scratch.arena, path);
  HANDLE file = CreateFileA(cpath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
  b32 result = (file != INVALID_HANDLE_VALUE);
  if(result)
  {
    CloseHandle(file);
  }
  scratch_end(&scratch);
  return result;
}

function b32
os_file_delete(String8 path)
{
  Scratch scratch = scratch_begin(0, 0);
  u8 *cpath = cstring_from_string8(scratch.arena, path);
  b32 result = DeleteFileA(cpath);
  scratch_end(&scratch);
  return result;
}

function b32
os_file_exists(String8 path)
{
  Scratch scratch = scratch_begin(0, 0);
  u8 *cpath = cstring_from_string8(scratch.arena, path);
  DWORD attr = GetFileAttributesA(cpath);
  scratch_end(&scratch);
  b32 result = (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY));
  return result;
}

function u32
os_file_overwrite(String8 path, u8* data, u64 data_size)
{
  Scratch scratch = scratch_begin(0, 0);
  u8 *cpath = cstring_from_string8(scratch.arena, path);
  HANDLE file = CreateFileA(cpath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
  scratch_end(&scratch);

  if(file == INVALID_HANDLE_VALUE)
  {
    return 0;
  }

  DWORD written = 0;
  WriteFile(file, data, (DWORD)data_size, &written, 0);
  CloseHandle(file);
  return (u32)written;
}

function u32
os_file_append(String8 path, u8* data, u64 data_size)
{
  Scratch scratch = scratch_begin(0, 0);
  u8 *cpath = cstring_from_string8(scratch.arena, path);
  HANDLE file = CreateFileA(cpath, FILE_APPEND_DATA, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
  scratch_end(&scratch);

  if(file == INVALID_HANDLE_VALUE)
  {
    return 0;
  }

  SetFilePointer(file, 0, 0, FILE_END);
  DWORD written = 0;
  WriteFile(file, data, (DWORD)data_size, &written, 0);
  CloseHandle(file);
  return (u32)written;
}

function b32
os_file_wipe(String8 path)
{
  Scratch scratch = scratch_begin(0, 0);
  u8 *cpath = cstring_from_string8(scratch.arena, path);
  HANDLE file = CreateFileA(cpath, GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
  scratch_end(&scratch);

  if(file == INVALID_HANDLE_VALUE)
  {
    return 0;
  }

  SetFilePointer(file, 0, 0, FILE_BEGIN);
  b32 result = SetEndOfFile(file);
  CloseHandle(file);
  return result;
}

function u32
os_file_size(String8 path)
{
  Scratch scratch = scratch_begin(0, 0);
  u8 *cpath = cstring_from_string8(scratch.arena, path);
  WIN32_FILE_ATTRIBUTE_DATA attr;
  b32 ok = GetFileAttributesExA(cpath, GetFileExInfoStandard, &attr);
  scratch_end(&scratch);

  if(!ok)
  {
    return 0;
  }

  ULARGE_INTEGER size;
  size.LowPart = attr.nFileSizeLow;
  size.HighPart = attr.nFileSizeHigh;
  return (u32)size.QuadPart;
}

function File_Data
os_file_load(Arena* arena, String8 path)
{
  File_Data result = {0};

  Scratch scratch = scratch_begin(0, 0);
  u8 *cpath = cstring_from_string8(scratch.arena, path);
  HANDLE file = CreateFileA(cpath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
  scratch_end(&scratch);

  if(file == INVALID_HANDLE_VALUE)
  {
    return result;
  }

  DWORD file_size = GetFileSize(file, 0);
  u8 *buffer = push_array(arena, u8, file_size);
  DWORD read = 0;
  if(ReadFile(file, buffer, file_size, &read, 0) && read == file_size)
  {
    result.path = path;
    result.data.str = buffer;
    result.data.size = file_size;
  }

  CloseHandle(file);
  return result;
}

function u64
os_file_get_last_modified_time(String8 path)
{
  Scratch scratch = scratch_begin(0, 0);
  u8 *cpath = cstring_from_string8(scratch.arena, path);
  HANDLE file = CreateFileA(cpath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
  scratch_end(&scratch);

  if(file == INVALID_HANDLE_VALUE)
  {
    return 0;
  }

  FILETIME ft;
  b32 success = GetFileTime(file, 0, 0, &ft);
  CloseHandle(file);

  if(!success)
  {
    return 0;
  }

  ULARGE_INTEGER time;
  time.LowPart  = ft.dwLowDateTime;
  time.HighPart = ft.dwHighDateTime;
  return time.QuadPart;
}

///////////////////////////////////////////////////////
// @Section: Directories

function b32
os_directory_create(String8 path)
{
  Scratch scratch = scratch_begin(0, 0);
  u8 *cpath = cstring_from_string8(scratch.arena, path);
  b32 result = CreateDirectoryA(cpath, 0);
  b32 exists = (GetLastError() == ERROR_ALREADY_EXISTS);
  scratch_end(&scratch);
  return result || exists;
}

function b32
os_directory_delete(String8 path)
{
  Scratch scratch = scratch_begin(0, 0);
  u8 *cpath = cstring_from_string8(scratch.arena, path);
  b32 result = RemoveDirectoryA(cpath);
  scratch_end(&scratch);
  return result;
}

function b32
os_directory_move(String8 path, String8 destination)
{
  Scratch scratch = scratch_begin(0, 0);
  u8 *csrc = cstring_from_string8(scratch.arena, path);
  u8 *cdst = cstring_from_string8(scratch.arena, destination);
  b32 result = MoveFileA(csrc, cdst);
  scratch_end(&scratch);
  return result;
}

function String8  
os_directory_pop(String8 path)
{
  String8 result = S("");
  for(u64 i = path.size; i > 0; i -= 1)
  {
    if(path.str[i - 1] == '/' || path.str[i - 1] == '\\')
    {
      result = string8_slice(path, 0, i - 1);
      break;
    }
  }
  return result;
}

function String8  
os_directory_push(String8 path, String8 directory)
{
  Scratch scratch = scratch_begin(0, 0);
  b32 needs_slash = (path.size > 0 &&
                    path.str[path.size - 1] != '/' &&
                    path.str[path.size - 1] != '\\');

  String8 slash = S("/");
  String8 result;

  if(needs_slash)
  {
    String8 temp = string8_concat(scratch.arena, path, slash);
    result = string8_concat(scratch.arena, temp, directory);
  }
  else
  {
    result = string8_concat(scratch.arena, path, directory);
  }

  scratch_end(&scratch);
  return result;
}

///////////////////////////////////////////////////////
// @Section: Misc
function String8
os_executable_path(Arena* arena)
{
  String8 result = {0};

  u8 temp_path[MAX_PATH];
  DWORD length = GetModuleFileNameA(NULL, (char*)temp_path, MAX_PATH);

  if (length > 0 && length < MAX_PATH)
  {
    result.size = (u64)length;
    result.str = push_array(arena, u8, result.size);
    MemoryCopy(result.str, temp_path, result.size);
  }

  return result;
}

function String8
os_get_appdata_dir(Arena* arena, String8 project_name)
{
  String8 result = {0};

  DWORD len = GetEnvironmentVariableA("APPDATA", 0, 0);
  if (len == 0) return result;

  u8* temp = push_array(arena, u8, len + 1);
  GetEnvironmentVariableA("APPDATA", (LPSTR)temp, len + 1);
  String8 appdata = string8_from_cstring(temp);

  // Build full path: %APPDATA%\project_name
  String8 full_path = string8_from_format(arena, "%S\\%S", appdata, project_name);

  // Create the directory if it doesn't exist
  os_directory_create(full_path);

  result = full_path;
  return result;
}


function void
os_exit_process(u32 code)
{
  ExitProcess(code);
}

///////////////////////////////////////////////////////
// @Section: OS Global State
///////////////////////////////////////////////////////
// @Section: Entry point (Windows specific functions behind entry_point)
int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
  SetUnhandledExceptionFilter(&win32_exception_filter);
  _g_hInstance = hInstance;

  // Dynamically load windows functions which are not guaranteed in all SDKs
  {
    HMODULE module = LoadLibraryA("kernel32.dll");
    g_win32_set_thread_description_func = (W32_SetThreadDescription_Type *)GetProcAddress(module, "SetThreadDescription");
    FreeLibrary(module);
  }

  local_persist Thread_Context thread_context;
  thread_context_init_and_attach(&thread_context);

  main_thread_base_entry_point(__argc, __argv);
  return _g_application_return;
}

///////////////////////////////////////////////////////
// @Section: Error handling

function LONG WINAPI
win32_exception_filter(EXCEPTION_POINTERS* exception_ptrs)
{
  // TODO(fz): Implement
  os_exit_process(1);
  return 1;
}

function void
_win32_output_last_error(DWORD err)
{
  LPWSTR messageBuffer = NULL;
  FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&messageBuffer, 0, NULL);

  if (messageBuffer)
  {
    // Convert wide string to narrow string (UTF-8 or ANSI) for OutputDebugStringA
    int size_needed = WideCharToMultiByte(CP_ACP, 0, messageBuffer, -1, NULL, 0, NULL, NULL);
    if (size_needed > 0)
    {
      char* messageA = (char*)HeapAlloc(GetProcessHeap(), 0, size_needed);
      if (messageA)
      {
        WideCharToMultiByte(CP_ACP, 0, messageBuffer, -1, messageA, size_needed, NULL, NULL);
        printf(": ");
        printf("%s\n", messageA);
        printf("\n");
        HeapFree(GetProcessHeap(), 0, messageA);
      }
    }
    LocalFree(messageBuffer);
  }
}

///////////////////////////////////////////////////////
// @Section: Timer

function void
os_time_init()
{
  QueryPerformanceFrequency(&g_win32_performance_frequency);
}

function u64
os_time_microseconds()
{
  LARGE_INTEGER counter;
  QueryPerformanceCounter(&counter);
  return (counter.QuadPart * 1000000) / g_win32_performance_frequency.QuadPart;
}

function u64
os_time_milliseconds()
{
  LARGE_INTEGER counter;
  QueryPerformanceCounter(&counter);
  return (counter.QuadPart * 1000) / g_win32_performance_frequency.QuadPart;
}

function f64
os_time_seconds()
{
  LARGE_INTEGER counter;
  QueryPerformanceCounter(&counter);
  return (f64)counter.QuadPart / (f64)g_win32_performance_frequency.QuadPart;
}

function u64
os_get_epoch_microseconds()
{
  FILETIME ft;
  GetSystemTimeAsFileTime(&ft);
  ULARGE_INTEGER uli = {ft.dwLowDateTime, ft.dwHighDateTime};
  return (uli.QuadPart - 116444736000000000ULL) / 10;
}

function OS_Date_Time
os_datetime_now()
{
  SYSTEMTIME st;
  GetLocalTime(&st);
  OS_Date_Time result =
  {
    st.wYear, st.wMonth, st.wDay,
    st.wHour, st.wMinute, st.wSecond,
    st.wMilliseconds
  };
  return result;
}

function String8
os_datetime_to_string8(Arena *arena, OS_Date_Time dt)
{
  local_persist u8* months[] = 
  {
    "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"
  };
 
  String8 month_name = (dt.month >= 1 && dt.month <= 12) ? string8_from_cstring(months[dt.month - 1]) : S("Invalid");
  
  String8 result = string8_from_format(arena, "%04u-%.*s-%02u %02u:%02u:%02u.%03u",
    dt.year, month_name.size, month_name.str, dt.day,dt.hour, dt.minute, dt.second, dt.millisecond);
                             
  return result;
}

function OS_Timer
os_timer_start()
{
  OS_Timer timer;
  QueryPerformanceCounter((LARGE_INTEGER*)&timer.opaque[0]);
  return timer;
}

function u64
os_timer_microseconds(OS_Timer *timer)
{
  LARGE_INTEGER now;
  QueryPerformanceCounter(&now);
  u64 start = timer->opaque[0];
  return ((now.QuadPart - start) * 1000000) / g_win32_performance_frequency.QuadPart;
}

function u64
os_timer_milliseconds(OS_Timer *timer)
{
  LARGE_INTEGER now;
  QueryPerformanceCounter(&now);
  u64 start = timer->opaque[0];
  return ((now.QuadPart - start) * 1000) / g_win32_performance_frequency.QuadPart;
}

function f64
os_timer_seconds(OS_Timer *timer)
{
  LARGE_INTEGER now;
  QueryPerformanceCounter(&now);
  u64 start = timer->opaque[0];
  return (f64)(now.QuadPart - start) / (f64)g_win32_performance_frequency.QuadPart;
}

function void
os_timer_reset(OS_Timer *timer)
{
  QueryPerformanceCounter((LARGE_INTEGER*)&timer->opaque[0]);
}