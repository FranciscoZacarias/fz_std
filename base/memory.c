
function Arena*
arena_alloc()
{
  Arena* arena = arena_alloc_sized(ARENA_RESERVE_SIZE, ARENA_COMMIT_SIZE);
  return arena;
}

function Arena*
arena_alloc_sized(u64 reserve, u64 commit)
{
  void* memory = NULL;
  
  u64 page_size = os_memory_get_page_size();
  reserve = AlignPow2(reserve, page_size);
  commit  = AlignPow2(commit,  page_size);
  
  Assert(ARENA_HEADER_SIZE < commit && commit <= reserve);
  
  memory = os_memory_reserve(reserve);
  if(!os_memory_commit(memory, commit))
  {
    memory = NULL;
    os_memory_release(memory, reserve);
  }
  
  Arena* arena = (Arena*) memory;
  
  if (arena)
  {
    arena->reserved    = reserve;
    arena->commited    = commit;
    arena->commit_size = commit;
    arena->position    = ARENA_HEADER_SIZE;
    arena->align       = DEFAULT_ALIGNMENT;
  }
  else
  {
    printf("Error setting arena's memory");
  }
  
  return arena;
}

function void*
_arena_push(Arena* arena, u64 size)
{
  void* result = _arena_push_no_zero(arena, size);
  MemoryZero(result, size);
  return result;
}

function void*
_arena_push_no_zero(Arena* arena, u64 size)
{
  void* result = NULL;

  if (arena->position + size <= arena->reserved)
  {
    u64 position_memory = AlignPow2(arena->position, arena->align);
    u64 new_position    = position_memory + size;
  
    if (arena->commited < new_position)
    {
      u64 commit_aligned = AlignPow2(new_position, arena->commit_size);
      u64 commit_clamped = ClampTop(commit_aligned, arena->reserved);
      u64 commit_size    = commit_clamped - arena->commited;
      if (os_memory_commit((u8*)arena + arena->commited, commit_size))
      {
        arena->commited = commit_clamped;
      }
      else
      {
        printf("Could not commit memory when increasing the arena's committed memory.");
      }
    }
    result = (u8*)arena + position_memory;
    arena->position = new_position;
  }
  else
  {
    printf("Trying to allocate too much memory to a non dynamic arena.\nSize: %llu\nArena->Position: %llu\nArena->reserved: %llu\nArena->Position+Size: %llu", size, arena->position, arena->reserved, arena->position+size);
  }
  
  return result;
}

function void
arena_pop(Arena* arena, u64 size)
{
  if (size > arena->position)
  {
    printf("Warning :: Arena :: Trying to pop %lld bytes from arena with %lld allocated. Will pop %lld instead of %lld.\n", size, arena->position, arena->position, size);
    size = arena->position;
  }
  arena->position -= size;
}

function void
arena_pop_to(Arena* arena, u64 pos)
{
  if (pos > arena->reserved)
  {
    printf("Warning :: Arena :: Trying to pop over arena's reserved. Will pop only to %lld instead of %lld", arena->reserved, pos);
    pos = arena->reserved;
  }
  else if (pos < 0)
  {
    printf("Warning :: Arena :: Trying to pop negative values. Will pop to 0");
    pos = 0;
  }
  arena->position = pos;
}

function void
arena_clear(Arena* arena)
{
  arena_pop(arena, arena->position);
}

function void
arena_free(Arena* arena)
{
  os_memory_release((u8*)arena, arena->reserved);
}

function void
print_arena(Arena *arena, const u8* label)
{
  f32 committed_percentage = ((f64)arena->position / arena->commited) * 100.0f;
  f32 reserved_percentage  = ((f64)arena->position / arena->reserved) * 100.0f;
  printf("%s: Arena { reserved: %llu, commited: %llu, commit_size: %llu, position: %llu, align: %llu, committed_percentage: %.2f%%, reserved_percentage: %.2f%% }\n",
         label, arena->reserved, arena->commited, arena->commit_size, arena->position, arena->align, committed_percentage, reserved_percentage);
}

function Scratch
arena_temp_begin(Arena* arena)
{
  Scratch temp;
  temp.arena = arena;
  temp.temp_position = arena->position;
  return temp;
}

function void
arena_temp_end(Scratch* temp)
{
  arena_pop_to(temp->arena, temp->temp_position);
}
