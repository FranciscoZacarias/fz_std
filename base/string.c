function String8
string8_new(u64 size, u8* str)
{
  String8 result = { size, str };
  return result;
}

function String8
string8_copy(Arena* arena, String8 source)
{
  String8 result;
  result.size = source.size;
  result.str  = push_array(arena, u8, result.size);
  MemoryCopy(result.str, source.str, result.size);
  return result;
}

function String8
string8_range(u8* first, u8* range)
{
  String8 result = (String8){(u64)(range - first), first};
  return result;
}

function String8
string8_concat(Arena* arena, String8 a, String8 b)
{
  String8 result = { 0 };
  result.size = a.size + b.size;
  result.str = push_array(arena, u8, result.size);
  MemoryCopy(result.str, a.str, a.size);
  MemoryCopy(result.str + a.size, b.str, b.size);
  return result;
}

function String8
string8_replace_first(Arena* arena, String8 str, String8 a, String8 b)
{
  u64 index = 0;
  if (!string8_find_first(str, a, &index))
  {
    return string8_copy(arena, str);
  }

  u64 new_size = str.size - a.size + b.size;
  u8* mem = (u8*)arena_push(arena, new_size);
  memcpy(mem, str.str, index);
  memcpy(mem + index, b.str, b.size);
  u64 after_size = str.size - (index + a.size);
  memcpy(mem + index + b.size, str.str + index + a.size, after_size);

  return (String8){ new_size, mem };
}

function String8
string8_replace_all(Arena *arena, String8 str, String8 a, String8 b)
{
  if (a.size == 0) return string8_copy(arena, str);

  String8 result = string8_copy(arena, str);
  u64 index = 0;

  while (string8_find_first(result, a, &index))
  {
    u64 new_size = result.size - a.size + b.size;
    u8 *mem = (u8 *)arena_push(arena, new_size);

    memcpy(mem, result.str, index);
    memcpy(mem + index, b.str, b.size);
    u64 after_size = result.size - (index + a.size);
    memcpy(mem + index + b.size, result.str + index + a.size, after_size);

    result.str = mem;
    result.size = new_size;
  }

  return result;
}


function b32
string8_match(String8 a, String8 b, b32 case_sensitive)
{
  if(a.size != b.size)
  {
    return false;
  }

  for(u64 i = 0; i < a.size; i += 1)
  {
    u8 ca = a.str[i];
    u8 cb = b.str[i];

    if(!case_sensitive)
    {
      if(ca >= 'A' && ca <= 'Z') ca += 32;
      if(cb >= 'A' && cb <= 'Z') cb += 32;
    }

    if(ca != cb)
    {
      return false;
    }
  }

  return true;
}

function String8
string8_slice(String8 str, u64 start, u64 end)
{
  if (start > str.size) start = str.size;
  if (end > str.size)   end   = str.size;
  if (start > end)      start = end;
  String8 result = (String8){ .size = end - start, .str  = str.str + start };
  return result;
}

function b32
string8_find_first(String8 str, String8 substring, u64* index)
{
  if (substring.size == 0 || substring.size > str.size)
  {
    return 0;
  }

  for (u64 i = 0; i <= str.size - substring.size; i++)
  {
    if (MemoryMatch(str.str + i, substring.str, substring.size))
    {
      if (index) *index = i;
      return 1;
    }
  }

  return 0;
}

function b32
string8_find_last(String8 str, String8 substring, u64* index)
{
  if (substring.size > str.size) return 0;
  b32 result = false;
  *index = U64_MAX;
  for (u64 i = str.size - substring.size + 1; i-- > 0;)
  {
    if (MemoryMatch(&str.str[i], substring.str, substring.size))
    {
      *index = i;
      result = true;
      break;
    }
  }
  return result;
}

function void
print(String8 str)
{
  printf(S_FMT, (s32)str.size, str.str);
}

function String8
string8_from_format(Arena* arena, char const* fmt, ...)
{
  String8 result = {0};

  va_list args;
  va_start(args, fmt);

  // Try to format into a fixed buffer first
  char temp[Kilobytes(16)];
  int len = vsnprintf(temp, sizeof(temp), fmt, args);
  va_end(args);

  if (len <= 0)
  {
    return result;
  }

  result.size = (u64)len;
  result.str = push_array(arena, u8, result.size);
  MemoryCopy(result.str, (u8*)temp, result.size);

  return result;
}

function u64
string8_hash(String8 str)
{
  u64 hash = 5381;
  for (u32 i = 0; i < str.size; i += 1)
  {
    hash = ((hash << 5) + hash) + (u8)(str.str[i]);
  }
  hash ^= str.size;
  return hash;
}

function String8_List
string8_split(Arena* arena, String8 str, String8 delimiter)
{
  String8_List result = {0};
  if (delimiter.size == 0)
  {
    printf("string8_split: delimiter must not be empty\n");
    return result;
  }

  u8* cursor = str.str;
  u8* end    = str.str + str.size;

  while (cursor < end)
  {
    u8* match = NULL;

    for (u8* scan = cursor; scan + delimiter.size <= end; scan++)
    {
      if (memcmp(scan, delimiter.str, delimiter.size) == 0)
      {
        match = scan;
        break;
      }
    }

    if (match)
    {
      string8_list_push(arena, &result, string8_range(cursor, match));

      cursor = match + delimiter.size;
    }
    else
    {
      string8_list_push(arena, &result, string8_range(cursor, end));
      break;
    }
  }

  return result;
}

function String8_List
string8_list_new(Arena* arena, String8 str)
{
  String8_List result = {0};
  String8_Node* node = push_array(arena, String8_Node, 1);
  result.first = node;
  result.last  = node;
  result.node_count = 1;
  result.total_size = node->value.size;
  return result;
}

function String8
string8_list_pop(String8_List* list)
{
  String8 result = {0};
  if (list->node_count < 1)  return result;
  
  String8_Node* last_node = list->last;
  result            = last_node->value;
  list->total_size -= last_node->value.size;

  if (list->node_count == 1)
  {
    list->first = 0;
    list->last = 0;
    list->node_count = 0;
  }
  else
  {
    String8_Node* current = list->first;
    while (current->next != last_node)
    {
      current = current->next;
    }
    current->next = 0;
    list->last = current;
    list->node_count -= 1;
  }

  return result;
}

function void
string8_list_push(Arena* arena, String8_List* list, String8 str)
{
  String8_Node* node = push_array(arena, String8_Node, 1);
  node->value = str;
  if (!list->first && !list->last)
  {
    list->first = node;
    list->last  = node;
  }
  else
  {
    list->last->next = node;
    list->last       = node;
  }
  list->node_count += 1;
  list->total_size += node->value.size;
}

function String8
string8_list_join(Arena* arena, String8_List* list)
{
  u8* dst = push_array(arena, u8, list->total_size);
  u8* ptr = dst;
  for (String8_Node* node = list->first; node; node = node->next)
  {
    MemoryCopy(ptr, node->value.str, node->value.size);
    ptr += node->value.size;
  }
  return string8_new(list->total_size, dst);
}

function b32
b32_from_string8(String8 str, b32* value)
{
  b32 result = 1;
  if (string8_match(str, S("false"), false))
  {
    *value = 0;
  }
  else if (string8_match(str, S("true"), false))
  {
    *value = 1;
  }
  else
  {
    result = 0;
  }
  return result;
}

function b32
f32_from_string8(String8 str, f32* value)
{
  *value = 0.0f;
  s32 decimal_position = -1;
  
  for (u64 i = 0; i < str.size; i++)
  {
    if (str.str[i] >= '0'  && str.str[i] <= '9')
    {
      *value = *value * 10.0f + (str.str[i] - '0');
      if (decimal_position != -1)
      {
        decimal_position += 1;
      }
    } 
    else if (str.str[i] == '.')
    {
      decimal_position = 0;
    }
    else
    {
      return 0;
    }
  }
  
  if (decimal_position != -1)
  {
    *value = *value / (f32)pow(10, decimal_position);
  }
  
  return 1;
}

function b32
s32_from_string8(String8 str, s32* value)
{
  *value = 0.0f;
  for (u64 i = 0; i < str.size; i++)
  {
    if (str.str[i] >= '0'  && str.str[i] <= '9')
    {
      *value = *value * 10.0f + (str.str[i] - '0');
    }
    else
    {
      return 0;
    }
  }
  return 1;
}

function u8*
cstring_from_string8(Arena* arena, String8 str)
{
  u8* result = push_array(arena, u8, str.size + 1);
  MemoryCopy(result, str.str, str.size);
  result[str.size] = 0;
  return result;
}

function String8
string8_from_cstring(u8* cstring)
{
  String8 result = string8_new((u64)strlen(cstring), cstring);
  return result;
}

function u32
cstring_length(u8* cstring)
{
  u32 result = 0;
  while (cstring[result] != '\0') { result += 1; }
  return result;
}

//~ Char Functions
function b32
char8_is_alpha(u8 c)
{
  return char8_is_alpha_upper(c) || char8_is_alpha_lower(c);
}

function b32
char8_is_alphanum(u8 c)
{
  return char8_is_alpha(c) || char8_is_digit(c);
}

function b32
char8_is_alpha_upper(u8 c)
{
  return c >= 'A' && c <= 'Z';
}

function b32
char8_is_alpha_lower(u8 c)
{
  return c >= 'a' && c <= 'z';
}

function b32
char8_is_digit(u8 c)
{
  return c >= '1' && c <= '9';
}

function b32
char8_is_symbol(u8 c)
{
  return (c == '~' || c == '!'  || c == '$' || c == '%' || c == '^' ||
          c == '&' || c == '*'  || c == '-' || c == '=' || c == '+' ||
          c == '<' || c == '.'  || c == '>' || c == '/' || c == '?' ||
          c == '|' || c == '\\' || c == '{' || c == '}' || c == '(' ||
          c == ')' || c == '\\' || c == '[' || c == ']' || c == '#' ||
          c == ',' || c == ';'  || c == ':' || c == '@');
}

function b32
char8_is_hexadecimal(u8 c)
{
  return char8_is_digit(c) ||
         (c >= 'A' && c <= 'F') ||
         (c >= 'a' && c <= 'f');
}

function b32
char8_is_space(u8 c)
{
  return c == ' ' || c == '\r' || c == '\t' || c == '\f' || c == '\v' || c == '\n';
}

function u8
char8_to_upper(u8 c)
{
  return (c >= 'a' && c <= 'z') ? ('A' + (c - 'a')) : c;
}

function u8
char8_to_lower(u8 c)
{
  return (c >= 'A' && c <= 'Z') ? ('a' + (c - 'A')) : c;
}