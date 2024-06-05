#include <Windows.h>
#include <userenv.h>

//~ OS entry point
#if IS_COMMAND_LINE_PROGRAM
int main(int argc, char** argv) {
  entry_main_thread(entry_point, (u64)__argc, __argv);
  return 0;
}
#else
int WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR lp_cmd_line, int n_show_cmd) {
  entry_main_thread(entry_point, (u64)__argc, __argv);
  return 0;
}
#endif // IS_COMMAND_LINE_PROGRAM

//~ Init
internal void
os_init() {
  Arena* arena = arena_alloc(Gigabytes(1));
  OSWin32State = PushArray(arena, OS_W32_State, 1);
  OSWin32State->arena = arena;
  OSWin32State->advapi_dll = LoadLibraryA("advapi32.dll");
  if(OSWin32State->advapi_dll) {
   *(FARPROC *)&RtlGenRandom = GetProcAddress(OSWin32State->advapi_dll, "SystemFunction036");
  }

  GetSystemInfo(&OSWin32State->system_info);
  QueryPerformanceFrequency(&OSWin32State->counts_per_second);

  InitializeSRWLock(&OSWin32State->process_srw_lock);
  InitializeSRWLock(&OSWin32State->thread_srw_lock);
  InitializeSRWLock(&OSWin32State->critical_section_srw_lock);
  InitializeSRWLock(&OSWin32State->srw_lock_srw_lock);
  InitializeSRWLock(&OSWin32State->condition_variable_srw_lock);
  OSWin32State->process_arena = arena_alloc(Kilobytes(256));
  OSWin32State->thread_arena = arena_alloc(Kilobytes(256));
  OSWin32State->critical_section_arena = arena_alloc(Kilobytes(256));
  OSWin32State->srw_lock_arena = arena_alloc(Kilobytes(256));
  OSWin32State->condition_variable_arena = arena_alloc(Kilobytes(256));

  {
    Arena_Temp scratch = ScratchBegin(0, 0);

    // rjf: gather binary path
    String binary_path = {0};
    {
      u64 size = Kilobytes(32);
      u16 *buffer = PushArrayNoZero(scratch.arena, u16, size);
      DWORD length = GetModuleFileNameW(0, (WCHAR*)buffer, (DWORD)size);
      binary_path = string_from_string16(scratch.arena, (String16){length, buffer});
      binary_path = string_path_chop_past_last_slash(binary_path);
    }

    // rjf: gather app data path
    String app_data_path = {0};
    {
      u64 size = Kilobytes(32);
      u16 *buffer = PushArrayNoZero(scratch.arena, u16, size);
      if(SUCCEEDED(SHGetFolderPathW(0, CSIDL_APPDATA, 0, 0, (WCHAR*)buffer))) {
        u16 *p = buffer;
        for (;*p; p += 1);
        String16 path16 = (String16){p - buffer, buffer};
        app_data_path = string_from_string16(scratch.arena, path16);
      }
    }

    // rjf: commit
    OSWin32State->binary_path = string_copy(arena, binary_path);
    OSWin32State->initial_path = OSWin32State->binary_path;
    OSWin32State->app_data_path = string_copy(arena, app_data_path);

    ScratchEnd(scratch);
  }

  OSWin32State->granular_sleep_enabled = (timeBeginPeriod(1) == TIMERR_NOERROR);
}

internal void
os_abort() {
  ExitProcess(1);
}

//~ Memory
internal void*
os_memory_reserve(u64 size) {
  void* result = VirtualAlloc(0, size, MEM_RESERVE, PAGE_NOACCESS);
  return result;
}

internal b32
os_memory_commit(void* memory, u64 size) {
  b32 result = (VirtualAlloc(memory, size, MEM_COMMIT, PAGE_READWRITE) != 0);
  return result;
}

internal void
os_memory_decommit(void* memory, u64 size) {
  VirtualFree(memory, size, MEM_DECOMMIT);
}

internal void
os_memory_release(void* memory, u64 size) {
  VirtualFree(memory, 0, MEM_RELEASE);
}

internal u64
os_memory_get_page_size() {
  SYSTEM_INFO sysinfo = {0};
  GetSystemInfo(&sysinfo);
  return(sysinfo.dwPageSize);
}

//~ File handling
internal OS_Handle
os_file_open(OS_Access_Flags access_flags, String path) {
  // unpack args
  Arena_Temp scratch = ScratchBegin(0, 0);
  String16 path16 = string16_from_string(scratch.arena, path);

  // map to w32 access flags
  DWORD desired_access = 0;
  if(access_flags & OS_AccessFlag_Read)  { desired_access |= GENERIC_READ; }
  if(access_flags & OS_AccessFlag_Write) { desired_access |= GENERIC_WRITE; }

  // create share mode
  DWORD share_mode = 0;
  if(access_flags & OS_AccessFlag_Shared) { share_mode = FILE_SHARE_READ; }

  // create security attributes
  SECURITY_ATTRIBUTES security_attributes = { (DWORD)sizeof(SECURITY_ATTRIBUTES), 0, 0, };

  // map to w32 creation disposition
  DWORD creation_disposition = 0;
  if(!(access_flags & OS_AccessFlag_CreateNew)) {
    creation_disposition = OPEN_EXISTING;
  }

  // misc.
  DWORD flags_and_attributes = 0;
  HANDLE template_file = 0;

  // open handle
  HANDLE file = CreateFileW((WCHAR*)path16.str,
                            desired_access,
                            share_mode,
                            &security_attributes,
                            creation_disposition,
                            flags_and_attributes,
                            template_file);

  // accumulate errors
  if(file != INVALID_HANDLE_VALUE)
  {
    // TODO(fz): append to errors
  }

  // map to abstract handle
  OS_Handle handle = {0};
  handle.u64[0] = (u64)file;

  ScratchEnd(scratch);
  return handle;
}

internal void
os_file_close(OS_Handle file) {
  HANDLE handle = (HANDLE)file.u64[0];
  if(handle != INVALID_HANDLE_VALUE) {
    CloseHandle(handle);
  }
}

internal String
os_file_read(Arena *arena, OS_Handle file, RingBuffer1U64 range) {
  String result = {0};
  HANDLE handle = (HANDLE)file.u64[0];
  LARGE_INTEGER off_li = {0};
  off_li.QuadPart = range.min;
  if(handle == INVALID_HANDLE_VALUE) {
    // TODO(fz): accumulate errors
  } else if(SetFilePointerEx(handle, off_li, 0, FILE_BEGIN)) {
    u64 bytes_to_read = dim1u64(range);
    u64 bytes_actually_read = 0;
    result.str = PushArrayNoZero(arena, u8, bytes_to_read);
    result.size = 0;
    u8 *ptr = result.str;
    u8 *opl = result.str + bytes_to_read;
    for(;;) {
      u64 unread = (u64)(opl - ptr);
      DWORD to_read = (DWORD)(ClampTop(unread, U32_MAX));
      DWORD did_read = 0;
      if(!ReadFile(handle, ptr, to_read, &did_read, 0)) {
        break;
      }
      ptr += did_read;
      result.size += did_read;
      if(ptr >= opl) {
        break;
      }
    }
  }
  return result;
}

internal void
os_file_write(OS_Handle file, u64 off, String_List data) {
  HANDLE handle = (HANDLE)file.u64[0];
  LARGE_INTEGER off_li = {0};
  off_li.QuadPart = off;
  if(handle == 0 || handle == INVALID_HANDLE_VALUE) {
    // TODO(fz): accumulate errors
  } else if(SetFilePointerEx(handle, off_li, 0, FILE_BEGIN)) {
    for(String_Node *node = data.first; node != 0; node = node->next) {
      u8 *ptr = node->value.str;
      u8 *opl = ptr + node->value.size;
      for(;;) {
        u64 unwritten = (u64)(opl - ptr);
        DWORD to_write = (DWORD)(ClampTop(unwritten, U32_MAX));
        DWORD did_write = 0;
        if(!WriteFile(handle, ptr, to_write, &did_write, 0)) {
          goto fail_out;
        }
        ptr += did_write;
        if(ptr >= opl) {
          break;
        }
      }
    }
  }
  fail_out:;
}

internal b32
os_file_is_valid(OS_Handle file) {
  HANDLE handle = (HANDLE)file.u64[0];
  return handle != INVALID_HANDLE_VALUE;

}

internal OS_File_Attributes os_attributes_from_file(OS_Handle file) {
  HANDLE handle = (HANDLE)file.u64[0];
  OS_File_Attributes atts = {0};
  u32 high_bits = 0;
  u32 low_bits = GetFileSize(handle, (DWORD *)&high_bits);
  FILETIME last_write_time = {0};
  GetFileTime(handle, 0, 0, &last_write_time);
  atts.size = (u64)low_bits | (((u64)high_bits) << 32);
  atts.last_modified = ((u64)last_write_time.dwLowDateTime) | (((u64)last_write_time.dwHighDateTime) << 32);
  return atts;
}

internal void os_delete_file(String path) {
  Arena_Temp scratch = ScratchBegin(0, 0);
  String16 path16 = string16_from_string(scratch.arena, path);
  DeleteFileW((WCHAR *)path16.str);
  ScratchEnd(scratch);
}

internal void os_move_file(String dst_path, String src_path) {
  Arena_Temp scratch = ScratchBegin(0, 0);
  String16 dst_path_16 = string16_from_string(scratch.arena, dst_path);
  String16 src_path_16 = string16_from_string(scratch.arena, src_path);
  MoveFileW((WCHAR *)src_path_16.str, (WCHAR *)dst_path_16.str);
  ScratchEnd(scratch);
}

internal b32 os_copy_file(String dst_path, String src_path) {
  Arena_Temp scratch = ScratchBegin(0, 0);
  String16 dst_path_16 = string16_from_string(scratch.arena, dst_path);
  String16 src_path_16 = string16_from_string(scratch.arena, src_path);
  b32 result = CopyFileW((WCHAR *)src_path_16.str, (WCHAR *)dst_path_16.str, 0);
  ScratchEnd(scratch);
  return result;
}

internal b32 os_make_directory(String path) {
  Arena_Temp scratch = ScratchBegin(0, 0);
  String16 path16 = string16_from_string(scratch.arena, path);
  b32 result = 1;
  if(!CreateDirectoryW((WCHAR *)path16.str, 0)) {
    DWORD error = GetLastError();
    if(error != ERROR_ALREADY_EXISTS) {
      result = 0;
    }
  }
  ScratchEnd(scratch);
  return result;
}

//~ Time
internal DateTime
os_date_time_now() {
  SYSTEMTIME st = {0};
  GetSystemTime(&st);
  DateTime dt = {0};
  {
    dt.year         = (u16)st.wYear;
    dt.month        = (u8)st.wMonth;
    dt.day_of_week  = (u8)st.wDayOfWeek;
    dt.day          = (u8)st.wDay;
    dt.hour         = (u8)st.wHour;
    dt.second       = (u8)st.wSecond;
    dt.milliseconds = (u16)st.wMilliseconds;
  }
  return dt;
}

internal u64 os_time_microseconds() {
  LARGE_INTEGER current_time;
  QueryPerformanceCounter(&current_time);
  f64 time_in_seconds = ((f64)current_time.QuadPart)/((f64)OSWin32State->counts_per_second.QuadPart);
  u64 time_in_microseconds = (u64)(time_in_seconds * Million(1));
  return time_in_microseconds;
}

internal void os_sleep(u64 milliseconds) {
  Sleep((DWORD)milliseconds);
}

internal void os_wait(u64 end_time_microseconds) {
  u64 begin_time_us = os_time_microseconds();
  if(end_time_microseconds > begin_time_us) {
    u64 time_to_wait_us = end_time_microseconds - begin_time_us;
    if(OSWin32State->granular_sleep_enabled) {
      os_sleep(time_to_wait_us/1000);
    } else {
      os_sleep(time_to_wait_us/15000);
    }
    for(;os_time_microseconds()<end_time_microseconds;);
  }
}

//~ File System Helpers

internal OS_File_Attributes
os_win32_file_attributes_from_find_data(WIN32_FIND_DATAW find_data) {
  OS_File_Attributes attributes = {0};
  if(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
    attributes.flags |= OS_FileFlag_Directory;
  }
  attributes.size = (((u64)find_data.nFileSizeHigh) << 32) | (find_data.nFileSizeLow);
  attributes.last_modified = (((u64)find_data.ftLastWriteTime.dwHighDateTime) << 32) | (find_data.ftLastWriteTime.dwLowDateTime);
  return attributes;
}

//~ Processes

internal OS_W32_Process* os_win32_process_alloc() {
  OS_W32_Process *result = 0;
  AcquireSRWLockExclusive(&OSWin32State->process_srw_lock);
  {
    result = OSWin32State->first_free_process;
    if(result != 0) {
      StackPop(OSWin32State->first_free_process);
    } else {
      result = PushArrayNoZero(OSWin32State->process_arena, OS_W32_Process, 1);
    }
    MemoryZeroStruct(result);
  }
  ReleaseSRWLockExclusive(&OSWin32State->process_srw_lock);
  return result;
}

internal void os_win32_process_release(OS_W32_Process* process) {
  AcquireSRWLockExclusive(&OSWin32State->process_srw_lock);
  {
    StackPush(OSWin32State->first_free_process, process);
  }
  ReleaseSRWLockExclusive(&OSWin32State->process_srw_lock);
}

//~ Threads

internal OS_W32_Thread*
os_win32_thread_alloc() {
  OS_W32_Thread *result = 0;
  AcquireSRWLockExclusive(&OSWin32State->thread_srw_lock);
  {
    result = OSWin32State->first_free_thread;
    if(result != 0) {
      StackPop(OSWin32State->first_free_thread);
    } else {
      result = PushArrayNoZero(OSWin32State->thread_arena, OS_W32_Thread, 1);
    }
    MemoryZeroStruct(result);
  }
  ReleaseSRWLockExclusive(&OSWin32State->thread_srw_lock);
  return result;
}

internal void
os_win32_thread_release(OS_W32_Thread* thread) {
  AcquireSRWLockExclusive(&OSWin32State->thread_srw_lock);
  {
    StackPush(OSWin32State->first_free_thread, thread);
  }
  ReleaseSRWLockExclusive(&OSWin32State->thread_srw_lock);
}

//~ Critical sections

internal OS_W32_CriticalSection*
os_win32_critical_section_alloc() {
  OS_W32_CriticalSection *result = 0;
  AcquireSRWLockExclusive(&OSWin32State->critical_section_srw_lock);
  {
    result = OSWin32State->first_free_critical_section;
    if(result != 0) {
      StackPop(OSWin32State->first_free_critical_section);
    } else {
      result = PushArrayNoZero(OSWin32State->critical_section_arena, OS_W32_CriticalSection, 1);
    }
    MemoryZeroStruct(result);
  }
  ReleaseSRWLockExclusive(&OSWin32State->critical_section_srw_lock);
  return result;
}

internal void
os_win32_critical_section_release(OS_W32_CriticalSection* critical_section) {
  AcquireSRWLockExclusive(&OSWin32State->critical_section_srw_lock);
  {
    StackPush(OSWin32State->first_free_critical_section, critical_section);
  }
  ReleaseSRWLockExclusive(&OSWin32State->critical_section_srw_lock);
}

//~ Srw locks

internal OS_W32_SRWLock*
os_win32_srw_lock_alloc() {
  OS_W32_SRWLock *result = 0;
  AcquireSRWLockExclusive(&OSWin32State->srw_lock_srw_lock);
  {
    result = OSWin32State->first_free_srw_lock;
    if(result != 0) {
      StackPop(OSWin32State->first_free_srw_lock);
    } else {
      result = PushArrayNoZero(OSWin32State->srw_lock_arena, OS_W32_SRWLock, 1);
    }
    MemoryZeroStruct(result);
  }
  ReleaseSRWLockExclusive(&OSWin32State->srw_lock_srw_lock);
  return result;
}

internal void
os_win32_srw_lock_release(OS_W32_SRWLock* srw_lock) {
  AcquireSRWLockExclusive(&OSWin32State->srw_lock_srw_lock);
  {
    StackPush(OSWin32State->first_free_srw_lock, srw_lock);
  }
  ReleaseSRWLockExclusive(&OSWin32State->srw_lock_srw_lock);
}

//~ Condition variables

internal OS_W32_ConditionVariable*
os_win32_conditional_variable_alloc() {
  OS_W32_ConditionVariable *cv = 0;
  AcquireSRWLockExclusive(&OSWin32State->condition_variable_srw_lock);
  {
    cv = OSWin32State->first_free_condition_variable;
    if(cv != 0) {
      StackPop(OSWin32State->first_free_condition_variable);
    } else {
      cv = PushArrayNoZero(OSWin32State->condition_variable_arena, OS_W32_ConditionVariable, 1);
    }
    MemoryZeroStruct(cv);
  }
  ReleaseSRWLockExclusive(&OSWin32State->condition_variable_srw_lock);
  return cv;
}

internal void
os_win32_conditional_variable_release(OS_W32_ConditionVariable* cv) {
  AcquireSRWLockExclusive(&OSWin32State->condition_variable_srw_lock);
  {
    StackPush(OSWin32State->first_free_condition_variable, cv);
  }
  ReleaseSRWLockExclusive(&OSWin32State->condition_variable_srw_lock);
}

//~ Thread entry point

internal DWORD
os_win32_thread_entry_point(void *params) {
  OS_W32_Thread *thread = (OS_W32_Thread *)params;
  entry_non_main_thread(thread->func, thread->params);
  return 0;
}

//~ Thread controls
internal u64 os_tid() {
  return GetThreadId(0);
}

internal void os_set_thread_name(String name) {
  Arena_Temp scratch = ScratchBegin(0, 0);
  String16 name16 = string16_from_string(scratch.arena, name);
  HRESULT hr = SetThreadDescription(GetCurrentThread(), (WCHAR*)name16.str);
  if(!SUCCEEDED(hr)) {
    int x = 42;
  }
  ScratchEnd(scratch);
}

internal OS_Handle os_thread_start(void* params, OS_ThreadFunction* func) {
  OS_W32_Thread *thread = os_win32_thread_alloc();
  if(thread != 0) {
    thread->params = params;
    thread->func = func;
    thread->handle = CreateThread(0, 0, os_win32_thread_entry_point, thread, 0, &thread->thread_id);
  }
  OS_Handle result = {(u64)(thread)};
  return result;
}

internal void os_thread_join(OS_Handle handle) {
  OS_W32_Thread *thread = (OS_W32_Thread *)handle.u64[0];
  if(thread != 0) {
    if(thread->handle != 0) {
      WaitForSingleObject(thread->handle, INFINITE);
      CloseHandle(thread->handle);
    }
    os_win32_thread_release(thread);
  }
}

internal void os_thread_detach(OS_Handle handle) {
  OS_W32_Thread *thread = (OS_W32_Thread *)handle.u64[0];
  if(thread != 0) {
    if(thread->handle != 0) {
      CloseHandle(thread->handle);
    }
    os_win32_thread_release(thread);
  }
}

//~ Mutexes

internal OS_Handle
os_mutex_alloc() {
  OS_W32_CriticalSection *critical_section = os_win32_critical_section_alloc();
  InitializeCriticalSection(&critical_section->base);
  OS_Handle handle = {(u64)critical_section};
  return handle;
}

internal void
os_mutex_release(OS_Handle handle) {
  OS_W32_CriticalSection *critical_section = (OS_W32_CriticalSection *)handle.u64[0];
  DeleteCriticalSection(&critical_section->base);
  os_win32_critical_section_release(critical_section);
}

internal void
os_mutex_scope_enter(OS_Handle handle) {
  OS_W32_CriticalSection *critical_section = (OS_W32_CriticalSection *)handle.u64[0];
  EnterCriticalSection(&critical_section->base);
}

internal void
os_mutex_scope_leave(OS_Handle handle) {
  OS_W32_CriticalSection *critical_section = (OS_W32_CriticalSection *)handle.u64[0];
  LeaveCriticalSection(&critical_section->base);
}

//~ Slim reader/writer mutexes

internal OS_Handle os_srw_mutex_alloc() {
  OS_W32_SRWLock *lock = os_win32_srw_lock_alloc();
  InitializeSRWLock(&lock->lock);
  OS_Handle h = {(u64)lock};
  return h;
}

internal void os_srw_mutex_release(OS_Handle handle) {
  OS_W32_SRWLock *lock = (OS_W32_SRWLock *)handle.u64[0];
  os_win32_srw_lock_release(lock);
}

internal void os_srw_mutex_scope_enter_w(OS_Handle handle) {
  OS_W32_SRWLock *lock = (OS_W32_SRWLock *)handle.u64[0];
  AcquireSRWLockExclusive(&lock->lock);
}

internal void os_srw_mutex_scope_leave_w(OS_Handle handle) {
  OS_W32_SRWLock *lock = (OS_W32_SRWLock *)handle.u64[0];
  ReleaseSRWLockExclusive(&lock->lock);
}

internal void os_srw_mutex_scope_enter_r(OS_Handle handle) {
  OS_W32_SRWLock *lock = (OS_W32_SRWLock *)handle.u64[0];
  AcquireSRWLockShared(&lock->lock);
}

internal void os_srw_mutex_scope_leave_r(OS_Handle handle) {
  OS_W32_SRWLock *lock = (OS_W32_SRWLock *)handle.u64[0];
  ReleaseSRWLockShared(&lock->lock);
}

//~ Semaphores

internal OS_Handle
os_semaphore_alloc(u32 initial_count, u32 max_count) {
  max_count = ClampTop(max_count, U32_MAX/2);
  initial_count = ClampTop(initial_count, max_count);
  HANDLE handle = CreateSemaphoreA(0, initial_count, max_count, 0);
  OS_Handle result = {0};
  result.u64[0] = (u64)handle;
  return result;
}

internal void
os_semaphore_release(OS_Handle handle) {
  HANDLE h = (HANDLE)handle.u64[0];
  CloseHandle(h);
}

internal b32
os_semaphore_wait(OS_Handle handle, u32 max_milliseconds) {
  HANDLE h = (HANDLE)handle.u64[0];
  DWORD wait_result = WaitForSingleObject(h, (max_milliseconds == U32_MAX ? INFINITE : max_milliseconds));
  b32 result = (wait_result == WAIT_OBJECT_0);
  return result;
}

internal u64
os_semaphore_signal(OS_Handle handle) {
  u32 count = 0;
  HANDLE h = (HANDLE)handle.u64[0];
  ReleaseSemaphore(h, 1, (LONG *)(&count));
  u64 result = (u64)count;
  return result;
}

//~ Condition variables

internal OS_Handle
os_conditional_veriable_alloc() {
  OS_W32_ConditionVariable *cv = os_win32_conditional_variable_alloc();
  OS_Handle handle = {(u64)cv};
  return handle;
}

internal void
os_conditional_veriable_release(OS_Handle cv_handle) {
  OS_W32_ConditionVariable *cv = (OS_W32_ConditionVariable *)cv_handle.u64[0];
  os_win32_conditional_variable_release(cv);
}

internal b32
os_conditional_veriable_wait(OS_Handle cv_handle, OS_Handle mutex_handle, u64 endt_us) {
  OS_W32_ConditionVariable *cv = (OS_W32_ConditionVariable *)cv_handle.u64[0];
  OS_W32_CriticalSection *crit_section = (OS_W32_CriticalSection *)mutex_handle.u64[0];
  u64 begint_us = os_time_microseconds();
  b32 result = 0;
  if(endt_us > begint_us) {
    u64 microseconds_to_wait = endt_us - begint_us;
    u64 milliseconds_to_wait = microseconds_to_wait / 1000;
    if(endt_us == U64_MAX) {
      milliseconds_to_wait = (u64)INFINITE;
    }
    result = !!SleepConditionVariableCS(&cv->base, &crit_section->base, (u32)milliseconds_to_wait);
  }
  return result;
}

internal b32
os_conditional_veriable_wait_srw_W(OS_Handle cv_handle, OS_Handle mutex_handle, u64 endt_us) {
  OS_W32_ConditionVariable *cv = (OS_W32_ConditionVariable *)cv_handle.u64[0];
  OS_W32_SRWLock *srw = (OS_W32_SRWLock *)mutex_handle.u64[0];
  u64 begint_us = os_time_microseconds();
  b32 result = 0;
  if(endt_us > begint_us) {
    u64 microseconds_to_wait = endt_us - begint_us;
    u64 milliseconds_to_wait = microseconds_to_wait / 1000;
    if(endt_us == U64_MAX) {
      milliseconds_to_wait = (u64)INFINITE;
    }
    result = !!SleepConditionVariableSRW(&cv->base, &srw->lock, (u32)milliseconds_to_wait, 0);
  }
  return result;
}

internal b32
os_conditional_veriable_wait_srw_R(OS_Handle cv_handle, OS_Handle mutex_handle, u64 endt_us) {
  OS_W32_ConditionVariable *cv = (OS_W32_ConditionVariable *)cv_handle.u64[0];
  OS_W32_SRWLock *srw = (OS_W32_SRWLock *)mutex_handle.u64[0];
  u64 begint_us = os_time_microseconds();
  b32 result = 0;
  if(endt_us > begint_us) {
    u64 microseconds_to_wait = endt_us - begint_us;
    u64 milliseconds_to_wait = microseconds_to_wait / 1000;
    if(endt_us == U64_MAX) {
      milliseconds_to_wait = (u64)INFINITE;
    }
    result = !!SleepConditionVariableSRW(&cv->base, &srw->lock, (u32)milliseconds_to_wait, CONDITION_VARIABLE_LOCKMODE_SHARED);
  }
  return result;
}

internal void
os_conditional_veriable_signal(OS_Handle cv_handle) {
  OS_W32_ConditionVariable *cv = (OS_W32_ConditionVariable *)cv_handle.u64[0];
  WakeConditionVariable(&cv->base);
}

internal void
os_conditional_veriable_signal_all(OS_Handle cv_handle) {
  OS_W32_ConditionVariable *cv = (OS_W32_ConditionVariable *)cv_handle.u64[0];
  WakeAllConditionVariable(&cv->base);
}
