
#ifndef F_MEMORY_H
#define F_MEMORY_H

#if !defined(ARENA_COMMIT_GRANULARITY)
#define ARENA_COMMIT_GRANULARITY Kilobytes(4)
#endif

#if !defined(ARENA_DECOMMIT_THRESHOLD)
#define ARENA_DECOMMIT_THRESHOLD Megabytes(64)
#endif

typedef struct Arena Arena;
struct Arena {
  u64 pos;
  u64 commit_pos;
  u64 align;
  u64 size;
  Arena* ptr;
  u64 __padding[3];
};

typedef struct Arena_Temp {
  Arena *arena;
  u64 pos;
} Arena_Temp;

internal Arena *arena_alloc(u64 size);
internal Arena *arena_alloc_default();
internal void   arena_release(Arena *arena);
internal void  *arena_push_no_zero(Arena *arena, u64 size);
internal void  *arena_push_aligned(Arena *arena, u64 alignment);
internal void  *arena_push(Arena *arena, u64 size);
internal void   arena_pop_to(Arena *arena, u64 pos);
internal void   arena_set_auto_align(Arena *arena, u64 align);
internal void   arena_pop(Arena *arena, u64 size);
internal void   arena_clear(Arena *arena);
internal u64    arena_pos(Arena *arena);

#define PushArrayNoZero(arena, type, count) (type*)arena_push_no_zero((arena), sizeof(type)*(count))
#define PushArray(arena, type, count)       (type*)arena_push((arena), sizeof(type)*(count))

internal Arena_Temp arena_temp_begin(Arena *arena);
internal void arena_temp_end(Arena_Temp temp);

#endif // F_MEMORY_H