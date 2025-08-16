#ifndef THREAD_CONTEXT_H
#define THREAD_CONTEXT_H

#define DEFAULT_ARENAS_PER_THREAD_CONTEXT 2

typedef struct Thread_Context Thread_Context;

struct Thread_Context {
  Arena* arenas[DEFAULT_ARENAS_PER_THREAD_CONTEXT];
};

C_LINKAGE thread_static Thread_Context* ThreadContextThreadLocal = 0;
global Thread_Context g_main_thread_context;

function void            thread_context_init_and_attach(Thread_Context* thread_context); /* Initializes a thread context with DEFAULT_ARENAS_PER_THREAD_CONTEXT arenas */
function void            thread_context_free(); /* Frees the thread context */
function Thread_Context* thread_context_get_equipped(); /* Returns current rhead context */
function Arena*         _thread_context_get_scratch(Arena** conflicts, u64 count); /* Returns a scratch arena */

#define scratch_begin(conflicts, count) arena_temp_begin(_thread_context_get_scratch((conflicts), (count)))
#define scratch_end(scratch) arena_temp_end(scratch)

#endif // THREAD_CONTEXT_H