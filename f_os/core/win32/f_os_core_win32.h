#ifndef F_OS_WIN32
#define F_OS_WIN32

#include <windows.h>
#include <windowsx.h>
#include <timeapi.h>
#include <tlhelp32.h>
#include <Shlobj.h>

////////////////////////////////
//~ Processes / Threads

typedef struct OS_W32_Process OS_W32_Process;
struct OS_W32_Process {
  OS_W32_Process *next;
  HANDLE parent_read;
  PROCESS_INFORMATION info;
  OS_ProcessStatus status;
};

typedef struct OS_W32_Thread OS_W32_Thread;
struct OS_W32_Thread {
  OS_W32_Thread *next;
  HANDLE handle;
  DWORD thread_id;
  void *params;
  OS_ThreadFunction *func;
};

////////////////////////////////
//~ Synchronization Primitives

typedef struct OS_W32_CriticalSection OS_W32_CriticalSection;
struct OS_W32_CriticalSection {
  OS_W32_CriticalSection *next;
  CRITICAL_SECTION base;
};

typedef struct OS_W32_SRWLock OS_W32_SRWLock;
struct OS_W32_SRWLock {
  OS_W32_SRWLock *next;
  SRWLOCK lock;
};

typedef struct OS_W32_ConditionVariable OS_W32_ConditionVariable;
struct OS_W32_ConditionVariable {
  OS_W32_ConditionVariable *next;
  CONDITION_VARIABLE base;
};

typedef struct OS_W32_State {
  Arena* arena;

  HMODULE advapi_dll;
  SYSTEM_INFO system_info;
  LARGE_INTEGER counts_per_second;
  b32 granular_sleep_enabled;
  String initial_path;
  String binary_path;
  String app_data_path;

  // process entity state
  SRWLOCK process_srw_lock;
  Arena *process_arena;
  OS_W32_Process *first_free_process;

  // thread entity state
  SRWLOCK thread_srw_lock;
  Arena *thread_arena;
  OS_W32_Thread *first_free_thread;

  // critical section entity state
  SRWLOCK critical_section_srw_lock;
  Arena *critical_section_arena;
  OS_W32_CriticalSection *first_free_critical_section;

  // srw lock entity state
  SRWLOCK srw_lock_srw_lock;
  Arena *srw_lock_arena;
  OS_W32_SRWLock *first_free_srw_lock;

  // condition variable entity state
  SRWLOCK condition_variable_srw_lock;
  Arena *condition_variable_arena;
  OS_W32_ConditionVariable *first_free_condition_variable;

} OS_W32_State;

global HINSTANCE OSWin32HInstance;
global OS_W32_State* OSWin32State;
BOOL (*RtlGenRandom)(VOID *RandomBuffer, ULONG RandomBufferLength);

//~ File System Helpers

internal OS_File_Attributes os_win32_file_attributes_from_find_data(WIN32_FIND_DATAW find_data);

//- Processes
internal OS_W32_Process *os_win32_process_alloc();
internal void os_win32_process_release(OS_W32_Process *process);

//- Threads
internal OS_W32_Thread* os_win32_thread_alloc();
internal void os_win32_thread_release(OS_W32_Thread* thread);

//- Critical sections
internal OS_W32_CriticalSection* os_win32_critical_section_alloc();
internal void os_win32_critical_section_release(OS_W32_CriticalSection* critical_section);

//- Srw locks
internal OS_W32_SRWLock* os_win32_srw_lock_alloc();
internal void os_win32_srw_lock_release(OS_W32_SRWLock* srw_lock);

//- Condition variables
internal OS_W32_ConditionVariable* os_win32_conditional_variable_alloc();
internal void os_win32_conditional_variable_release(OS_W32_ConditionVariable* cv);

//~ Thread Entry Point

internal DWORD os_win32_thread_entry_point(void *params);

#endif // F_OS_WIN32