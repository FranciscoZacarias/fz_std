internal void application_stop() {
  IsApplicationRunning = false;
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
internal HANDLE _win32_get_file_handle_read(String8 file_path) {
  Arena_Temp scratch = scratch_begin(0,0);
  char8* cstring = cstring_from_string8(scratch.arena, file_path);
  HANDLE file_handle = CreateFileA(cstring, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (file_handle == INVALID_HANDLE_VALUE) {
    DWORD error = GetLastError();
    printf("Error: Failed to open file %s. Error: %lu\n", file_path.str, error);
    return NULL;
  }
  scratch_end(&scratch);
  return file_handle;
}

internal HANDLE _win32_get_file_handle_write(String8 file_path) {
  Arena_Temp scratch = scratch_begin(0,0);
  char8* cstring = cstring_from_string8(scratch.arena, file_path);
  HANDLE file_handle = CreateFileA(cstring, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (file_handle == INVALID_HANDLE_VALUE) {
    DWORD error = GetLastError();
    printf("Error: Failed to open file %s. Error: %lu\n", file_path.str, error);
    return NULL;
  }
  scratch_end(&scratch);
  return file_handle;
}

internal b32 file_create(String8 file_path) {
  Arena_Temp scratch = scratch_begin(0, 0);
  b32 result = 0;
  if (file_exists(file_path)) {
    return result;
  }

  char8* cstring = cstring_from_string8(scratch.arena, file_path);
  HANDLE file = CreateFileA(cstring, GENERIC_READ, 0, 0, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
  DWORD error = GetLastError();  
  if (error == ERROR_SUCCESS || error == ERROR_FILE_EXISTS) {
    result = 1;
  } else {
    // TODO(fz): We should send this error to user space
    printf("Error creating file %s with error: %lu\n", file_path.str, error);
  }
  CloseHandle(file);
  scratch_end(&scratch);
  return result;
}

internal u64 file_get_last_modified_time(String8 file_path) {
  u32 result = 0;
  WIN32_FILE_ATTRIBUTE_DATA file_attribute_data;
  if (GetFileAttributesExA(file_path.str, GetFileExInfoStandard, &file_attribute_data)) {
    FILETIME last_write_time = file_attribute_data.ftLastWriteTime;
    result = (((u64)last_write_time.dwHighDateTime << 32) | ((u64)last_write_time.dwLowDateTime));
  }
  return result;
}

internal b32 file_has_extension(String8 filename, String8 ext) {
  if (filename.size < ext.size + 1)  return false;
  if (filename.str[filename.size - ext.size - 1] != '.')  return false;
  for (u64 i = 0; i < ext.size; i++) {
    if (tolower(filename.str[filename.size - ext.size + i]) != tolower(ext.str[i])) {
      return false;
    }
  }
  return true;
}

internal String8_List file_get_all_file_paths_recursively(Arena* arena, String8 path) {
  String8_List result = {0};
  Arena_Temp scratch = scratch_begin(0, 0);

  if (!path_is_directory(path)) {
    printf("Path '%s' is not a directory.\n", path.str);
    scratch_end(&scratch);
    return result;
  }

  String8_List dir_queue = {0};
  string8_list_push(scratch.arena, &dir_queue, path);

  while (dir_queue.node_count > 0) {
    String8 current_dir = string8_list_pop(&dir_queue);
    String16 current_dir16 = string16_from_string8(scratch.arena, current_dir);
    if (current_dir16.size == 0) continue;

    // search_path = current_dir16 + L"\*"
    String16 search_path = {
      current_dir16.size + 2,
      ArenaPushNoZero(scratch.arena, wchar_t, current_dir16.size + 3)
    };
    memcpy(search_path.str, current_dir16.str, current_dir16.size * sizeof(wchar_t));
    search_path.str[current_dir16.size]     = L'\\';
    search_path.str[current_dir16.size + 1] = L'*';
    search_path.str[current_dir16.size + 2] = L'\0';

    WIN32_FIND_DATAW find_data;
    HANDLE find_handle = FindFirstFileW(search_path.str, &find_data);
    if (find_handle == INVALID_HANDLE_VALUE) continue;

    do {
      if (wcscmp(find_data.cFileName, L".") == 0 || wcscmp(find_data.cFileName, L"..") == 0) {
        continue;
      }

      String16 filename16 = {0, find_data.cFileName};
      while (filename16.str[filename16.size] != L'\0') filename16.size++;

      String8 filename8 = string8_from_string16(scratch.arena, filename16);
      if (filename8.size == 0) continue;

      u64 full_path_size = current_dir.size + 1 + filename8.size;
      char8* full_path_str = ArenaPushNoZero(scratch.arena, char8, full_path_size + 1);
      memcpy(full_path_str, current_dir.str, current_dir.size);
      full_path_str[current_dir.size] = '\\';
      memcpy(full_path_str + current_dir.size + 1, filename8.str, filename8.size);
      full_path_str[full_path_size] = '\0';

      String8 full_path = {full_path_size, full_path_str};
      b32 is_directory = HasFlags(find_data.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY);

      if (is_directory) {
        string8_list_push(scratch.arena, &dir_queue, full_path);
      } else {
        String8 normalized_path = path_new(arena, full_path);
        string8_list_push(arena, &result, normalized_path);
      }

    } while (FindNextFileW(find_handle, &find_data));

    FindClose(find_handle);
  }

  scratch_end(&scratch);
  return result;
}

internal String8 path_new(Arena* arena, String8 input) {
  char8* data = ArenaPush(arena, char8, input.size);
  for (u64 i = 0; i < input.size; i++) {
    char8 c = input.str[i];
    data[i] = (c == '/' || c == '\\') ? '\\' : c;
  }
  return (String8){ .size = input.size, .str = data };
}

internal b32 path_create_as_directory(String8 path) {
  b32 result = false;
  char8 buffer[MAX_PATH];
  u64 len = (path.size < MAX_PATH - 1) ? path.size : (MAX_PATH - 1);
  MemoryCopy(buffer, path.str, len);
  buffer[len] = 0;

  // Try to create the directory
  if (CreateDirectoryA(buffer, 0)) {
    result = true;
  } else {
    // If it already exists, consider it success
    DWORD err = GetLastError();
    if (err == ERROR_ALREADY_EXISTS || err == ERROR_ACCESS_DENIED) {
      result = true;
    }
  }

  return result;
}

internal b32 path_is_file(String8 path) {
  b32 result = false;
  char8 buffer[MAX_PATH];
  u64 len = (path.size < MAX_PATH - 1) ? path.size : (MAX_PATH - 1);
  MemoryCopy(buffer, path.str, len);
  buffer[len] = 0;

  DWORD attrs = GetFileAttributesA(buffer);
  if (attrs == INVALID_FILE_ATTRIBUTES) {
    return result;
  }
  result = (attrs & FILE_ATTRIBUTE_DIRECTORY) == 0;
  return result;
}

internal b32 path_is_directory(String8 path) {
  b32 result = false;

  if (path.size == 0 || path.str == NULL) {
    return result;
  }
  
  Arena_Temp scratch = scratch_begin(0, 0);
  String16 path16 = string16_from_string8(scratch.arena, path);
  if (path16.size == 0) {
    scratch_end(&scratch);
    return result;
  }
  
  wchar_t *wcstr   = wcstr_from_string16(scratch.arena, path16);
  DWORD attributes = GetFileAttributesW(wcstr);
  scratch_end(&scratch);

  if (attributes == INVALID_FILE_ATTRIBUTES) {
    return result;
  }

  result = (b32)(attributes & FILE_ATTRIBUTE_DIRECTORY);
  return result;;
}

internal String8 path_get_working_directory(void) {
  static char8 buffer[MAX_PATH];
  DWORD len = GetCurrentDirectoryA(MAX_PATH, buffer);
  if (len == 0 || len >= MAX_PATH) {
    return (String8){0};
  }
  return (String8){ .size = len, .str = buffer };
}

internal String8 path_get_file_name(String8 path) {
  u64 last_sep = 0;
  for (u64 i = 0; i < path.size; i++) {
    if (path.str[i] == '\\' || path.str[i] == '/') {
      last_sep = i + 1;
    }
  }
  return (String8){
    .str = path.str + last_sep,
    .size = path.size - last_sep
  };
}

internal String8 path_get_file_name_no_ext(String8 path) {
  String8 file = path_get_file_name(path);

  u64 dot = file.size;
  for (u64 i = 0; i < file.size; i++) {
    if (file.str[i] == '.') {
      dot = i;
      break;
    }
  }

  return (String8){
    .str = file.str,
    .size = dot
  };
}

internal String8 path_join(Arena* arena, String8 a, String8 b) {
  b32 a_ends_with_sep   = (a.size > 0 && (a.str[a.size - 1] == '/' || a.str[a.size - 1] == '\\'));
  b32 b_starts_with_sep = (b.size > 0 && (b.str[0] == '/' || b.str[0] == '\\'));

  u64 total_size = a.size + b.size + 1; // +1 in case we need to insert a separator
  if (a_ends_with_sep && b_starts_with_sep) {
    total_size -= 1; // we’ll skip one of the slashes
  }

  char8* data = ArenaPush(arena, char8, total_size);
  u64 pos = 0;

  // Copy 'a'
  MemoryCopy(data + pos, a.str, a.size);
  pos += a.size;

  // Handle slash insertion/removal
  if (!a_ends_with_sep && !b_starts_with_sep && a.size > 0 && b.size > 0) {
    data[pos++] = '\\';
  } else if (a_ends_with_sep && b_starts_with_sep) {
    b.str += 1;
    b.size -= 1;
  }

  // Copy 'b'
  MemoryCopy(data + pos, b.str, b.size);
  pos += b.size;

  return (String8){ .size = pos, .str = data };
}

internal String8 path_get_current_directory_name(String8 path) {
  String8 result = {0};
  u64 index = 0;
  if (string8_find_last(path, Str8("\\"), &index)) {
    result = string8_slice(path, index+1, path.size);
  }
  return result;
}

internal String8 path_dirname(String8 path) {
  u64 last_sep = 0;
  for (u64 i = 0; i < path.size; i++) {
    if (path.str[i] == '/' || path.str[i] == '\\') {
      last_sep = i;
    }
  }

  return (String8){
    .size = last_sep,
    .str  = path.str,
  };
}

internal b32 file_exists(String8 file_path) {
  b32 result = 0;
  Arena_Temp scratch = scratch_begin(0,0);
  char8* cpath = cstring_from_string8(scratch.arena, file_path);
  DWORD file_attributes = GetFileAttributesA(cpath);
  if (file_attributes == INVALID_FILE_ATTRIBUTES) {
    printf("File %s attributes are invalid.\n", cpath);
  } else if (file_attributes & FILE_ATTRIBUTE_DIRECTORY) {
    printf("Path %s is a directory.\n", cpath);
  } else {
    result = true;
  }
  scratch_end(&scratch);
  return result;
}


internal u32 file_overwrite(String8 file_path, char8* data, u64 data_size) {
  Arena_Temp scratch = scratch_begin(0, 0);
  char8* cpath = cstring_from_string8(scratch.arena, file_path);

  // Create or overwrite the file in one shot
  HANDLE file_handle = CreateFileA(cpath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

  if (file_handle == INVALID_HANDLE_VALUE) {
    DWORD error = GetLastError();
    printf("CreateFileA failed: error code %lu for file %s\n", error, cpath);
    scratch_end(&scratch);
    return 0;
  }

  DWORD bytes_written = 0;
  BOOL write_result = WriteFile(file_handle, data, (DWORD)data_size, &bytes_written, NULL);
  if (!write_result) {
    DWORD error = GetLastError();
    printf("WriteFile failed: error code %lu\n", error);
    CloseHandle(file_handle);
    scratch_end(&scratch);
    return 0;
  }

  CloseHandle(file_handle);
  scratch_end(&scratch);
  return bytes_written;
}

internal u32 file_append(String8 file_path, char8* data, u64 data_size) {
  Arena_Temp scratch = scratch_begin(0, 0);
  char8* cpath = cstring_from_string8(scratch.arena, file_path);
  HANDLE file_handle = CreateFileA(cpath, FILE_APPEND_DATA | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

  if (file_handle == INVALID_HANDLE_VALUE) {
    DWORD error = GetLastError();
    printf("CreateFileA failed in file_append: error code %lu for file %s\n", error, cpath);
    scratch_end(&scratch);
    return 0;
  }

  // Move file pointer to the end manually (some versions of FILE_APPEND_DATA don't imply this)
  SetFilePointer(file_handle, 0, NULL, FILE_END);

  DWORD bytes_written = 0;
  BOOL result = WriteFile(file_handle, data, (DWORD)data_size, &bytes_written, NULL);
  if (!result) {
    DWORD error = GetLastError();
    printf("WriteFile failed in file_append: error code %lu\n", error);
    CloseHandle(file_handle);
    scratch_end(&scratch);
    return 0;
  }

  CloseHandle(file_handle);
  scratch_end(&scratch);
  return bytes_written;
}

internal b32 file_wipe(String8 file_path) {
  if (!file_exists(file_path)) {
    return true;
  }
  
  HANDLE file_handle = _win32_get_file_handle_write(file_path);
  if (file_handle == INVALID_HANDLE_VALUE) {
    return false;
  }
  
  if (!SetEndOfFile(file_handle)) {
    CloseHandle(file_handle);
    return false;
  }
  
  CloseHandle(file_handle);
  return true;
}

internal u32 file_size(String8 file_path) {
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

internal File_Data file_load(Arena* arena, String8 file_path) {
  File_Data result = { 0 };
  if (!file_exists(file_path)) {
    printf("Error: file_load failed because file %s doesn't exist\n", file_path.str);
    return result;
  }

  HANDLE file_handle = _win32_get_file_handle_read(file_path);
  if (file_handle == NULL) {
    return result;
  }
  
  u32 size    = file_size(file_path);
  char8* data = ArenaPush(arena, char8, size);
  MemoryZero(data, size);

  if (!ReadFile(file_handle, data, size, NULL, NULL)) {
    DWORD error = GetLastError();  
    printf("Error: %lu in file_load.\n", error);
    return result;
  }
  result.path = file_path;
  result.data.str = data;
  result.data.size = size;
  
  CloseHandle(file_handle);
  return result;
}

internal void file_list_push(Arena* arena, File_List* list, File_Data file) {
  File_Node* node = ArenaPush(arena, File_Node, sizeof(File_Node));
  
  node->value = file;
  if (!list->first && !list->last) {
    list->first = node;
    list->last  = node;
  } else {
    list->last->next = node;
    list->last       = node;
  }
  list->node_count += 1;
  list->total_size += node->value.data.size;
}

internal void println_string(String8 string) {
  HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
  WriteFile(handle, string.str, string.size, NULL, NULL);
  char8 newline = '\n';
  WriteFile(handle, &newline, 1, NULL, NULL);
}

internal void _error_message_and_exit(const char8 *file, int line, const char8 *func, const char8 *fmt, ...) {
  char8 buffer[1024];
  va_list args;

  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);

  char8 detailed_buffer[2048];
  MemoryZero(detailed_buffer, 2048);
  s32 len = snprintf(detailed_buffer, sizeof(detailed_buffer), "Error at %s:%d in %s\n%s", file, line, func, buffer);
  
  if (ErrorLogFile.size > 0) {
    Arena_Temp scratch = scratch_begin(0,0);
    file_append(ErrorLogFile, detailed_buffer, len);
    scratch_end(&scratch);
  }

  printf(detailed_buffer);

  MessageBoxA(0, detailed_buffer, "ERROR: fz_std", MB_OK);
  ExitProcess(1);
}