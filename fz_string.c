internal String8 string8_new(u64 size, char8* str) {
  String8 result = { size, str };
  return result;
}

internal String8 string8_format(Arena* arena, String8 fmt, ...) {
  char8* c_fmt = cstring_from_string8(arena, fmt);

  va_list args;
  va_start(args, fmt);

  s32 required = vsnprintf(0, 0, c_fmt, args);
  va_end(args);

  if (required <= 0) {
    return (String8){0};
  }

  char8* buffer = ArenaPush(arena, char8, required + 1);

  va_start(args, fmt);
  vsnprintf(buffer, required + 1, c_fmt, args);
  va_end(args);

  return string8_new((u64)required, buffer);
}

internal String8 string8_range(char8* first, char8* range) {
  String8 result = (String8){(u64)(range - first), first};
  return result;
}

internal String8 string8_concat(Arena* arena, String8 a, String8 b) {
  String8 result = { 0 };
  result.size = a.size + b.size;
  result.str = ArenaPush(arena, char8, result.size);
  MemoryCopy(result.str, a.str, a.size);
  MemoryCopy(result.str + a.size, b.str, b.size);
  return result;
}

internal b32 string8_equal(String8 a, String8 b) {
  if (a.size != b.size)  return false;
  for(u32 i = 0; i < a.size; i++) {
    if (a.str[i] != b.str[i])  return false;
  }
  return true;
}

internal String8 string8_slice(String8 str, u64 start, u64 end) {
  if (start > str.size) start = str.size;
  if (end > str.size)   end   = str.size;
  if (start > end)      start = end;
  return (String8){ .size = end - start, .str  = str.str + start };
}

internal b32 string8_find_last(String8 str, String8 substring, u64* index) {
  if (substring.size > str.size) return 0;
  b32 result = false;
  *index = U64_MAX;
  for (u64 i = str.size - substring.size + 1; i-- > 0;) {
    if (MemoryMatch(&str.str[i], substring.str, substring.size)) {
      *index = i;
      result = true;
      break;
    }
  }
  return result;
}

internal void string8_printf(String8 str) {
  printf("%.*s", (s32)str.size, str.str);
}


internal String8_List string8_split(Arena* arena, String8 str, String8 split_character) {
  String8_List result = { 0 };
  
  if (split_character.size != 1) {
    ERROR_MESSAGE_AND_EXIT("string8_split expects only one char8acter in split_character. It got %s of size %llu\n", split_character.str, split_character.size);
  }
  
  char8* cursor = str.str;
  char8* end    = str.str + str.size;
  for(; cursor < end; cursor++) {
    char8 byte  = *cursor;
    if (byte == split_character.str[0]) {
      string8_list_push(arena, &result, string8_range(str.str, cursor));
      string8_list_push(arena, &result, string8_range(cursor, end));
      break;
    }
  }
  
  return result;
}

internal String8_List string8_list_new(Arena* arena, String8 str) {
  String8_List result = {0};
  String8_Node* node = ArenaPush(arena, String8_Node, 1);
  result.first = node;
  result.last  = node;
  result.node_count = 1;
  result.total_size = node->value.size;
  return result;
}

internal String8 string8_list_pop(String8_List* list) {
  String8 result = {0};
  if (list->node_count < 1)  return result;
  
  String8_Node* last_node = list->last;
  result            = last_node->value;
  list->total_size -= last_node->value.size;

  if (list->node_count == 1) {
    list->first = 0;
    list->last = 0;
    list->node_count = 0;
  } else {
    String8_Node* current = list->first;
    while (current->next != last_node) {
      current = current->next;
    }
    current->next = 0;
    list->last = current;
    list->node_count -= 1;
  }

return result;
}

internal void string8_list_push(Arena* arena, String8_List* list, String8 str) {
  String8_Node* node = ArenaPush(arena, String8_Node, sizeof(String8_Node));
  
  node->value = str;
  if (!list->first && !list->last) {
    list->first = node;
    list->last  = node;
  } else {
    list->last->next = node;
    list->last       = node;
  }
  list->node_count += 1;
  list->total_size += node->value.size;
}

internal String8 string8_list_join(Arena* arena, String8_List* list) {
  char8* dst = ArenaPush(arena, char8, list->total_size);
  char8* ptr = dst;
  for (String8_Node* node = list->first; node; node = node->next) {
    MemoryCopy(ptr, node->value.str, node->value.size);
    ptr += node->value.size;
  }
  return string8_new(list->total_size, dst);
}

internal b32 b32_from_string8(String8 str, b32* value) {
  b32 result = 1;
  if (string8_equal(str, Str8("false"))) {
    *value = 0;
  } else if (string8_equal(str, Str8("true"))) {
    *value = 1;
  } else {
    result = 0;
  }
  return result;
}

internal b32 f32_from_string8(String8 str, f32* value) {
  *value = 0.0f;
  s32 decimal_position = -1;
  
  for (u64 i = 0; i < str.size; i++) {
    if (str.str[i] >= '0'  && str.str[i] <= '9') {
      *value = *value * 10.0f + (str.str[i] - '0');
      if (decimal_position != -1) {
        decimal_position += 1;
      }
    } else if (str.str[i] == '.') {
      decimal_position = 0;
    } else {
      return 0;
    }
  }
  
  if (decimal_position != -1) {
    *value = *value / (f32)pow(10, decimal_position);
  }
  
  return 1;
}

internal b32 s32_from_string8(String8 str, s32* value) {
  *value = 0.0f;
  for (u64 i = 0; i < str.size; i++) {
    if (str.str[i] >= '0'  && str.str[i] <= '9') {
      *value = *value * 10.0f + (str.str[i] - '0');
    } else {
      return 0;
    }
  }
  return 1;
}

internal char8* cstring_from_string8(Arena* arena, String8 str) {
  char8* result = ArenaPush(arena, char8, str.size + 1);
  MemoryCopy(result, str.str, str.size);
  result[str.size] = 0;
  return result;
}

internal String8 string8_from_cstring(char8* cstring) {
  String8 result = string8_new((u64)strlen(cstring), cstring);
  return result;
}

internal String16 string16_from_string8(Arena *arena, String8 str8) {
  String16 result = {0};
  s32 str16_length = MultiByteToWideChar(CP_UTF8, 0, str8.str, (s32)str8.size, NULL, 0);
  if (str16_length > 0) {
    result.size = str16_length;
    result.str  = ArenaPushNoZero(arena, char16, str16_length);
    MultiByteToWideChar(CP_UTF8, 0, str8.str, (s32)str8.size, (wchar_t*)result.str, str16_length);
  }
  return result;
}

internal String8 string8_from_string16(Arena* arena, String16 str16) {
  String8 result = {0};
  s32 len = WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)str16.str, (s32)str16.size, NULL, 0, NULL, NULL);
  if (len > 0) {
    result.size = len;
    result.str = ArenaPushNoZero(arena, char, len);
    WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)str16.str, (s32)str16.size, result.str, len, NULL, NULL);
  }
  return result;
}

internal wchar_t* wcstr_from_string16(Arena *arena, String16 str16) {
  wchar_t *wcstr = ArenaPushNoZero(arena, wchar_t, str16.size + 1);
  memcpy(wcstr, str16.str, str16.size * sizeof(char16));
  wcstr[str16.size] = L'\0';
  return wcstr;
}

//~ Char Functions
internal b32 char8_is_alpha(char8 c) {
  return char8_is_alpha_upper(c) || char8_is_alpha_lower(c);
}

internal b32 char8_is_alphanum(char8 c) {
  return char8_is_alpha(c) || char8_is_digit(c);
}

internal b32 char8_is_alpha_upper(char8 c) {
  return c >= 'A' && c <= 'Z';
}

internal b32 char8_is_alpha_lower(char8 c) {
  return c >= 'a' && c <= 'z';
}

internal b32 char8_is_digit(char8 c) {
  return c >= '1' && c <= '9';
}

internal b32 char8_is_symbol(char8 c) {
  return (c == '~' || c == '!'  || c == '$' || c == '%' || c == '^' ||
          c == '&' || c == '*'  || c == '-' || c == '=' || c == '+' ||
          c == '<' || c == '.'  || c == '>' || c == '/' || c == '?' ||
          c == '|' || c == '\\' || c == '{' || c == '}' || c == '(' ||
          c == ')' || c == '\\' || c == '[' || c == ']' || c == '#' ||
          c == ',' || c == ';'  || c == ':' || c == '@');
}

internal b32 char8_is_space(char8 c) {
  return c == ' ' || c == '\r' || c == '\t' || c == '\f' || c == '\v' || c == '\n';
}

internal char8 char8_to_upper(char8 c) {
  return (c >= 'a' && c <= 'z') ? ('A' + (c - 'a')) : c;
}

internal char8 char8_to_lower(char8 c) {
  return (c >= 'A' && c <= 'Z') ? ('a' + (c - 'A')) : c;
}