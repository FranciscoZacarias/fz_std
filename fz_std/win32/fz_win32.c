internal void application_stop() {
  PostQuitMessage(0);
}

internal void* memory_reserve(u64 size) {
  void* result = VirtualAlloc(0, size, MEM_RESERVE, PAGE_NOACCESS);
  return result;
}

internal b32 memory_commit(void* memory, u64 size) {
  b32 result = (VirtualAlloc(memory, size, MEM_COMMIT, PAGE_READWRITE) != 0);
  return result;
}

internal void  memory_decommit(void* memory, u64 size) {
  VirtualFree(memory, size, MEM_DECOMMIT);
}

internal void  memory_release(void* memory, u64 size) {
  VirtualFree(memory, size, MEM_RELEASE);
}

internal u64 memory_get_page_size() {
  SYSTEM_INFO sysinfo = {0};
  GetSystemInfo(&sysinfo);
  return(sysinfo.dwPageSize);
}

//~ File handling

internal HANDLE _win32_get_file_handle_read(String file_path) {
  HANDLE file_handle = CreateFileA(file_path.str, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (file_handle == INVALID_HANDLE_VALUE) {
    DWORD error = GetLastError();
    printf("Error: Failed to open file %s. Error: %lu\n", file_path.str, error);
    return NULL;
  }
  return file_handle;
}

internal HANDLE _win32_get_file_handle_write(String file_path) {
  HANDLE file_handle = CreateFileA(file_path.str, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (file_handle == INVALID_HANDLE_VALUE) {
    DWORD error = GetLastError();
    printf("Error: Failed to open file %s. Error: %lu\n", file_path.str, error);
    return NULL;
  }
  return file_handle;
}

internal b32 file_create(String file_path) {
  b32 result  = 0;
  HANDLE file = CreateFileA(file_path.str, GENERIC_READ, 0, 0, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
  DWORD error = GetLastError();  
  if (error == ERROR_SUCCESS || error == ERROR_FILE_EXISTS) {
    result = 1;
  } else {
    // TODO(fz): We should send this error to user space
    printf("Error creating file %s with error: %lu\n", file_path.str, error);
  }
  CloseHandle(file);
  return result;
}

internal u64 file_get_last_modified_time(String file_path) {
  u32 result = 0;
  WIN32_FILE_ATTRIBUTE_DATA file_attribute_data;
  if (GetFileAttributesExA(file_path.str, GetFileExInfoStandard, &file_attribute_data)) {
    FILETIME last_write_time = file_attribute_data.ftLastWriteTime;
    result = (((u64)last_write_time.dwHighDateTime << 32) | ((u64)last_write_time.dwLowDateTime));
  }
  return result;
}

internal b32 file_exists(String file_path) {
  b32 result = 0;
  DWORD file_attributes = GetFileAttributesA(file_path.str);
  if (file_attributes == INVALID_FILE_ATTRIBUTES) {
    printf("File %s attributes are invalid.\n", file_path.str);
  } else if (file_attributes & FILE_ATTRIBUTE_DIRECTORY) {
    printf("Path %s is a directory.\n", file_path.str);
  } else {
    result = true;
  }
  return result;
}

internal u32 file_write(String file_path, u8* data, u64 data_size) {
  s32 bytes_written = 0;
  HANDLE file_handle = _win32_get_file_handle_write(file_path);

  if (!WriteFile(file_handle, data, data_size, &bytes_written, NULL)) {
    printf("WriteFile failed (error %d)\n", GetLastError());
  }

  return bytes_written;
}

internal u32 file_size(String file_path) {
  u32 result = 0;
  if (!file_exists(file_path)) {
    printf("Error: file_exists failed because file %s doesn't exist\n", file_path.str);
    return result;
  }
  WIN32_FILE_ATTRIBUTE_DATA file_attribute_data;
  if (GetFileAttributesExA(file_path.str, GetFileExInfoStandard, &file_attribute_data)) {
    result = ((u64)file_attribute_data.nFileSizeHigh << 32) | ((u64)file_attribute_data.nFileSizeLow);
  }
  return result;
}

internal File_Data file_load(Arena* arena, String file_path) {
  File_Data file = { 0 };
  if (!file_exists(file_path)) {
    printf("Error: file_load failed because file %s doesn't exist\n", file_path.str);
    return file;
  }

  HANDLE file_handle = _win32_get_file_handle_read(file_path);
  if (file_handle == NULL) {
    return file;
  }
  
  u32 size  = file_size(file_path);
  file.size = size;
  file.data = ArenaPush(arena, char, size);
  MemoryZero(file.data, file.size);

  if (!ReadFile(file_handle, file.data, size, NULL, NULL)) {
    DWORD error = GetLastError();  
    printf("Error: %lu in file_load.\n", error);
  }
  
  CloseHandle(file_handle);
  return file;
}

internal void print_string(String string) {
  HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
  WriteFile(handle, string.str, string.size, NULL, NULL);
  char newline = '\n';
  WriteFile(handle, &newline, 1, NULL, NULL);
}

internal void _error_message_and_exit(const char *file, int line, const char *func, const char *fmt, ...) {
  char buffer[1024];
  va_list args;

  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);

  char detailed_buffer[2048];
  snprintf(detailed_buffer, sizeof(detailed_buffer), "Error at %s:%d in %s\n%s", file, line, func, buffer);

  MessageBoxA(0, detailed_buffer, "ahah idiot", MB_OK);
  ExitProcess(1);
}