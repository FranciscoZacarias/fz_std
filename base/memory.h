#ifndef MEMORY_H
#define MEMORY_H

#ifndef ARENA_RESERVE_SIZE
# define ARENA_RESERVE_SIZE Gigabytes(1)
#endif
#ifndef ARENA_COMMIT_SIZE
# define ARENA_COMMIT_SIZE Kilobytes(64)
#endif

typedef struct Arena Arena;
struct Arena {
  u64 reserved;      // Reserved memory
  u64 commited;      // Commited memory
  u64 commit_size;   // Size for each commit on this arena
  u64 position;      // Current position of the arena
  u64 align;         // Arena's memory alignment
};
#define ARENA_HEADER_SIZE AlignPow2(sizeof(Arena), os_memory_get_page_size())

function Arena* arena_alloc();
function Arena* arena_alloc_sized(u64 reserve, u64 commit);

function void* _arena_push(Arena* arena, u64 size);
function void* _arena_push_no_zero(Arena* arena, u64 size);
function void   arena_pop(Arena* arena, u64 size);
function void   arena_pop_to(Arena* arena, u64 pos);
function void   arena_clear(Arena* arena);
function void   arena_free(Arena* arena);

function void print_arena(Arena *arena, const u8* label);

#define push_array(arena, type, count)         (type*)_arena_push((arena), sizeof(type)*(count))
#define push_array_no_zero(arena, type, count) (type*)_arena_push_no_zero((arena), sizeof(type)*(count))

typedef struct Scratch {
  Arena* arena;
  u64 temp_position;
} Scratch;

function Scratch arena_temp_begin(Arena* arena);
function void    arena_temp_end(Scratch* temp);

#endif // MEMORY_H