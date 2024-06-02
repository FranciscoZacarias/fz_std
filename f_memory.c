internal Arena*
arena_alloc(u64 size) {
  u64 size_roundup_granularity = Megabytes(64);
  size += size_roundup_granularity-1;
  size -= size%size_roundup_granularity;
  void* block = os_memory_reserve(size);
  u64 initial_commit_size = ARENA_COMMIT_GRANULARITY;
  Assert(initial_commit_size >= sizeof(Arena));
  os_memory_commit(block, initial_commit_size);
  Arena* arena = (Arena*)block;
  arena->pos = sizeof(Arena);
  arena->commit_pos = initial_commit_size;
  arena->align = 8;
  arena->size = size;
  return arena;
}

internal Arena*
arena_alloc_default() {
  Arena* arena = arena_alloc(Gigabytes(8));
  return arena;
}

internal void
arena_release(Arena* arena) {
  os_memory_release(arena, arena->size);
}

internal void *
arena_push_no_zero(Arena* arena, u64 size) {
  void* result = 0;
  if(arena->pos + size <= arena->size) {
    u8 *base = (u8 *)arena;
    u64 post_align_pos = (arena->pos + (arena->align-1));
    post_align_pos -= post_align_pos%arena->align;
    u64 align = post_align_pos - arena->pos;
    result = base + arena->pos + align;
    arena->pos += size + align;
    if(arena->commit_pos < arena->pos) {
      u64 size_to_commit = arena->pos - arena->commit_pos;
      size_to_commit += ARENA_COMMIT_GRANULARITY - 1;
      size_to_commit -= size_to_commit%ARENA_COMMIT_GRANULARITY;
      os_memory_commit(base + arena->commit_pos, size_to_commit);
      arena->commit_pos += size_to_commit;
    }
  } else {
    // TODO(fz): Fallback strategy
    Assert(0);
  }
  return result;
}

internal void *
arena_push_aligned(Arena *arena, u64 alignment) {
  u64 pos = arena->pos;
  u64 pos_rounded_up = pos + alignment-1;
  pos_rounded_up -= pos_rounded_up%alignment;
  u64 size_to_alloc = pos_rounded_up - pos;
  void *result = 0;
  if(size_to_alloc != 0) {
    result = arena_push_no_zero(arena, size_to_alloc);
  }
  return result;
}

internal void *
arena_push(Arena *arena, u64 size) {
  void *result = arena_push_no_zero(arena, size);
  MemoryZero(result, size);
  return result;
}

internal void
arena_pop_to(Arena *arena, u64 pos) {
  u64 min_pos = sizeof(Arena);
  u64 new_pos = Max(min_pos, pos);
  arena->pos = new_pos;
  u64 pos_aligned_to_commit_chunks = arena->pos + ARENA_COMMIT_GRANULARITY-1;
  pos_aligned_to_commit_chunks -= pos_aligned_to_commit_chunks%ARENA_COMMIT_GRANULARITY;
  if(pos_aligned_to_commit_chunks + ARENA_DECOMMIT_THRESHOLD <= arena->commit_pos) {
    u8 *base = (u8 *)arena;
    u64 size_to_decommit = arena->commit_pos-pos_aligned_to_commit_chunks;
    os_memory_decommit(base + pos_aligned_to_commit_chunks, size_to_decommit);
    arena->commit_pos -= size_to_decommit;
  }
}

internal void
arena_set_auto_align(Arena *arena, u64 align) {
  arena->align = align;
}

internal void
arena_pop(Arena *arena, u64 size) {
  u64 min_pos = sizeof(Arena);
  u64 size_to_pop = Min(size, arena->pos);
  u64 new_pos = arena->pos - size_to_pop;
  new_pos = Max(new_pos, min_pos);
  arena_pop_to(arena, new_pos);
}

internal void
arena_clear(Arena *arena) {
  arena_pop_to(arena, sizeof(Arena));
}

internal u64
arena_pos(Arena *arena) {
  return arena->pos;
}

internal Arena_Temp
arena_temp_begin(Arena *arena) {
  Arena_Temp temp = {0};
  temp.arena = arena;
  temp.pos = arena->pos;
  return temp;
}

internal void
arena_temp_end(Arena_Temp temp) {
  arena_pop_to(temp.arena, temp.pos);
}
