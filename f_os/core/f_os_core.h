#ifndef F_OS_CORE_H
#define F_OS_CORE_H

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

#endif // F_OS_CORE_H