#ifndef F_ENTRY_H
#define F_ENTRY_H

//~ Main thread entry point
internal void entry_main_thread(void (*entry_point)(u64 argc, char** argv), u64 argument_count, char **arguments);

//~ Non-main-thread entry point
internal void entry_non_main_thread(void (*entry_point)(void *p), void *params);

#endif // F_ENTRY_H