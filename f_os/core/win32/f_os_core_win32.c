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
  OSWin32State = PushArray(arena, OS_Win32_State, 1);
  OSWin32State->arena = arena;
  OSWin32State->advapi_dll = LoadLibraryA("advapi32.dll");
  if(OSWin32State->advapi_dll) {
   *(FARPROC *)&RtlGenRandom = GetProcAddress(OSWin32State->advapi_dll, "SystemFunction036");
  }

  GetSystemInfo(&OSWin32State->system_info);
  QueryPerformanceFrequency(&OSWin32State->counts_per_second);

  OSWin32State->granular_sleep_enabled = (timeBeginPeriod(1) == TIMERR_NOERROR);
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