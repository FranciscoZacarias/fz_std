#ifndef F_THREAD_CONTEXT_H
#define F_THREAD_CONTEXT_H

typedef struct Thread_Context {
  Arena* scratch_arenas[2];
  b32 is_main_thread;
  u8 thread_name[64];
  u64 thread_name_size;
} Thread_Context;

thread_static Thread_Context* _ThreadContextLocal = 0;

internal Thread_Context  thread_context_alloc();
internal void            thread_context_release(Thread_Context *tctx);
internal void            thread_context_set(Thread_Context *tctx);
internal Thread_Context *thread_context_get();
internal b32             is_main_thread();
internal void            thread_context_set_name(String string);
internal String          thread_context_get_name();

internal Arena_Temp ScratchBegin(Arena **conflicts, u64 conflict_count);
#define ScratchEnd(arena_temp) arena_temp_end(arena_temp)

#endif // F_THREAD_CONTEXT_H