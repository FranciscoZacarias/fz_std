
internal Thread_Context
thread_context_alloc() {
  Thread_Context tctx = {0};
  for(u64 arena_idx = 0; arena_idx < ArrayCount(tctx.scratch_arenas); arena_idx += 1) {
    tctx.scratch_arenas[arena_idx] = arena_alloc(Gigabytes(8));
  }
  return tctx;
}

internal void
thread_context_release(Thread_Context *tctx) {
  for(u64 arena_idx = 0; arena_idx < ArrayCount(tctx->scratch_arenas); arena_idx += 1) {
    arena_release(tctx->scratch_arenas[arena_idx]);
  }
}

internal void
thread_context_set(Thread_Context *tctx) {
  _ThreadContextLocal = tctx;
}

internal Thread_Context *
thread_context_get() {
  return _ThreadContextLocal;
}

internal b32
is_main_thread() {
  Thread_Context *tctx = thread_context_get();
  return tctx->is_main_thread;
}

internal void thread_context_set_name(String string) {
  Thread_Context* tctx = thread_context_get();
  tctx->thread_name_size = Min(string.size, sizeof(tctx->thread_name));
  MemoryCopy(tctx->thread_name, string.str, tctx->thread_name_size);
  // TODO(fz): Implement this at the OS layer. We need to also set thread name in the os thread
}

internal String thread_context_get_name() {
  Thread_Context* tctx = thread_context_get();
  String result = string_new(tctx->thread_name_size, tctx->thread_name);
  return result;
}

internal Arena_Temp
ScratchBegin(Arena **conflicts, u64 conflict_count) {
  Arena_Temp scratch = {0};
  Thread_Context *tctx = thread_context_get();
  for(u64 tctx_idx = 0; tctx_idx < ArrayCount(tctx->scratch_arenas); tctx_idx += 1) {
    b32 is_conflicting = 0;
    for(Arena **conflict = conflicts; conflict < conflicts+conflict_count; conflict += 1) {
      if(*conflict == tctx->scratch_arenas[tctx_idx]) {
        is_conflicting = 1;
        break;
      }
    }
    if(is_conflicting == 0) {
      scratch.arena = tctx->scratch_arenas[tctx_idx];
      scratch.pos = scratch.arena->pos;
      break;
    }
  }
  return scratch;
}
