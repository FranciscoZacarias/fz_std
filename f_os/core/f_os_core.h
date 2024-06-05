#ifndef F_OS_CORE_H
#define F_OS_CORE_H

typedef struct OS_Handle {
  u64 u64[1];
} OS_Handle;

typedef u64 OS_Timestamp;

//~ Filesystem types

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

//~ Thread & Process Types

typedef void OS_ThreadFunction(void *params);

typedef struct OS_ProcessStatus {
  b8 launch_failed;
  b8 running;
  b8 read_failed;
  b8 kill_failed;
  b8 was_killed;
  u32 exit_code;
} OS_ProcessStatus;

typedef struct OS_Stripe {
  Arena *arena;
  OS_Handle cv;
  OS_Handle mutex;
} OS_Stripe;

typedef struct OS_StripeTable {
  u64 count;
  OS_Stripe *stripes;
} OS_StripeTable;

//~ This function is the entry point. Implement this in the entry point of the program.
internal void entry_point(u64 argc, char** argv);

//~ Init
internal void os_init();
internal void os_abort();

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

//~ Thread controls
internal u64       os_tid();
internal void      os_set_thread_name(String name);
internal OS_Handle os_thread_start(void* params, OS_ThreadFunction* func);
internal void      os_thread_join(OS_Handle handle);
internal void      os_thread_detach(OS_Handle handle);

//~ Mutexes
internal OS_Handle os_mutex_alloc();
internal void os_mutex_release(OS_Handle handle);
internal void os_mutex_scope_enter(OS_Handle handle);
internal void os_mutex_scope_leave(OS_Handle handle);
#define OS_MutexScope(m) DeferLoop(os_mutex_scope_enter(m), os_mutex_scope_leave(m))

//~ Slim reader/writer mutexes
internal OS_Handle os_srw_mutex_alloc();
internal void os_srw_mutex_release(OS_Handle handle);
internal void os_srw_mutex_scope_enter_w(OS_Handle handle);
internal void os_srw_mutex_scope_leave_w(OS_Handle handle);
internal void os_srw_mutex_scope_enter_r(OS_Handle handle);
internal void os_srw_mutex_scope_leave_r(OS_Handle handle);
#define OS_SRWMutexScope_W(m) DeferLoop(os_srw_mutex_scope_enter_w(m), os_srw_mutex_scope_leave_w(m))
#define OS_SRWMutexScope_R(m) DeferLoop(os_srw_mutex_scope_enter_r(m), os_srw_mutex_scope_leave_r(m))

//~ Semaphores
internal OS_Handle os_semaphore_alloc(u32 initial_count, u32 max_count);
internal void      os_semaphore_release(OS_Handle handle);
internal b32       os_semaphore_wait(OS_Handle handle, u32 max_milliseconds);
internal u64       os_semaphore_signal(OS_Handle handle);

//~ Condition variables
internal OS_Handle os_conditional_veriable_alloc();
internal void      os_conditional_veriable_release(OS_Handle cv_handle);
internal b32       os_conditional_veriable_wait(OS_Handle cv_handle, OS_Handle mutex, u64 endt_us);
internal b32       os_conditional_veriable_wait_srw_W(OS_Handle cv_handle, OS_Handle mutex_handle, u64 endt_us);
internal b32       os_conditional_veriable_wait_srw_R(OS_Handle cv_handle, OS_Handle mutex_handle, u64 endt_us);
internal void      os_conditional_veriable_signal(OS_Handle cv_handle);
internal void      os_conditional_veriable_signal_all(OS_Handle cv_handle);

//~ Time
internal DateTime os_date_time_now();
internal u64 os_time_microseconds();
internal void os_sleep(u64 milliseconds);
internal void os_wait(u64 end_time_microseconds);

#endif // F_OS_CORE_H