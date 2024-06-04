#ifndef F_OS_CORE_H
#define F_OS_CORE_H

typedef struct OS_Handle {
  u64 u64[1];
} OS_Handle;

typedef u64 OS_Timestamp;

typedef u32 OS_File_Flags;
enum {
  OS_FileFlag_Directory = (1<<0),
};

typedef struct OS_File_Attributes {
  OS_File_Flags flags;
  u64 size;
  OS_Timestamp last_modified;
} OS_File_Attributes;

typedef u32 OS_Access_Flags;
enum {
  OS_AccessFlag_Read      = (1<<0),
  OS_AccessFlag_Write     = (1<<1),
  OS_AccessFlag_Execute   = (1<<2),
  OS_AccessFlag_CreateNew = (1<<3),
  OS_AccessFlag_Shared    = (1<<4),
};

//~ This function is the entry point. Implement this in the entry point of the program.
internal void entry_point(u64 argc, char** argv);

//~ Init
internal void os_init(void);

//~ Memory
internal void* os_memory_reserve(u64 size);
internal b32   os_memory_commit(void* memory, u64 size);
internal void  os_memory_decommit(void* memory, u64 size);
internal void  os_memory_release(void* memory, u64 size);
internal u64   os_memory_get_page_size();

//~ File handling
internal OS_Handle os_file_open(OS_Access_Flags access_flags, String path);
internal void      os_file_close(OS_Handle file);
internal String    os_file_read(Arena *arena, OS_Handle file, RingBuffer1U64 range);
internal void      os_file_write(OS_Handle file, u64 off, String_List data);
internal b32       os_file_is_valid(OS_Handle file);
internal OS_File_Attributes os_attributes_from_file(OS_Handle file);

internal void os_delete_file(String path);
internal void os_move_file(String dst_path, String src_path);
internal b32  os_copy_file(String dst_path, String src_path);
internal b32  os_make_directory(String path);

//~ Time
internal DateTime os_date_time_now();
internal u64 os_time_microseconds();
internal void os_sleep(u64 milliseconds);
internal void os_wait(u64 end_time_microseconds);

#endif // F_OS_CORE_H