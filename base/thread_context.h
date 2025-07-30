#ifndef THREAD_CONTEXT_H
#define THREAD_CONTEXT_H

typedef struct Thread_Context Thread_Context;
struct Thread_Context {
  Arena* arenas[2];
};

C_LINKAGE thread_static Thread_Context* ThreadContextThreadLocal = 0;
global Thread_Context g_main_thread_context;

function void            thread_context_init_and_attach(Thread_Context* thread_context);
function void            thread_context_free();
function Thread_Context* thread_context_get_equipped();
function Arena*          thread_context_get_scratch(Arena** conflicts, u64 count);

#define scratch_begin(conflicts, count) arena_temp_begin(thread_context_get_scratch((conflicts), (count)))
#define scratch_end(scratch) arena_temp_end(scratch)

#endif // THREAD_CONTEXT_H