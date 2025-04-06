internal String string_new(u64 size, char* str) {
  String result = { size, str };
  return result;
}

internal String string_format(String fmt, ...) {
  // TODO(fz): Implement
  return fmt;
}

internal String string_range(char* first, char* range) {
  String result = (String){(u64)(range - first), first};
  return result;
}

internal String string_concat(Arena* arena, String a, String b) { printf("%s not implemented", __func__);  return StringLiteral(""); };  // TODO(fz): Implement 
internal String string_trim(String str)                         { printf("%s not implemented", __func__);  return StringLiteral(""); } // TODO(fz): Implement 
internal b32    string_contains(String str, String substring)   { printf("%s not implemented", __func__);  return false; } // TODO(fz): Implement 
internal s64    string_find_first(String str, String substring) { printf("%s not implemented", __func__);  return 0; } // TODO(fz): Implement 
internal s64    string_find_last(String str, String substring)  { printf("%s not implemented", __func__);  return 0; } // TODO(fz): Implement 

internal b32 string_equal(String a, String b) {
  if (a.size != b.size) {
    return 0;
  }
  for(u32 i = 0; i < a.size; i++) {
    if (a.str[i] != b.str[i]) {
      return 0;
    }
  }
  return 1;
}

internal String_List string_split(Arena* arena, String str, String split_character) {
  String_List result = { 0 };
  
  if (split_character.size != 1) {
    ERROR_MESSAGE_AND_EXIT("string_split expects only one character in split_character. It got %s of size %llu\n", split_character.str, split_character.size);
  }
  
  char* cursor = str.str;
  char* end    = str.str + str.size;
  for(; cursor < end; cursor++) {
    char byte  = *cursor;
    if (byte == split_character.str[0]) {
      string_list_push(arena, &result, string_range(str.str, cursor));
      string_list_push(arena, &result, string_range(cursor, end));
      break;
    }
  }
  
  return result;
}

internal String string_list_pop(String_List* list)   { printf("%s not implemented", __func__);  return StringLiteral(""); };  // TODO(fz): Implement 
internal void   string_list_clear(String_List* list) { printf("%s not implemented", __func__); };  // TODO(fz): Implement 

internal void string_list_push(Arena* arena, String_List* list, String str) {
  String_Node* node = ArenaPush(arena, String_Node, sizeof(String_Node));
  
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

internal b32 cast_string_to_b32(String str, b32* value) {
  b32 result = 1;
  if (string_equal(str, StringLiteral("false"))) {
    *value = 0;
  } else if (string_equal(str, StringLiteral("true"))) {
    *value = 1;
  } else {
    result = 0;
  }
  return result;
}

internal b32 cast_string_to_f32(String str, f32* value) {
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

internal b32 cast_string_to_s32(String str, s32* value) {
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

internal b32 char_is_alpha(char c) {
  return char_is_alpha_upper(c) || char_is_alpha_lower(c);
}

internal b32 char_is_alpha_upper(char c) {
  return c >= 'A' && c <= 'Z';
}

internal b32 char_is_alpha_lower(char c) {
  return c >= 'a' && c <= 'z';
}

internal b32 char_is_digit(char c) {
  return c >= '1' && c <= '9';
}

internal b32 char_is_symbol(char c) {
  return (c == '~' || c == '!'  || c == '$' || c == '%' || c == '^' ||
          c == '&' || c == '*'  || c == '-' || c == '=' || c == '+' ||
          c == '<' || c == '.'  || c == '>' || c == '/' || c == '?' ||
          c == '|' || c == '\\' || c == '{' || c == '}' || c == '(' ||
          c == ')' || c == '\\' || c == '[' || c == ']' || c == '#' ||
          c == ',' || c == ';'  || c == ':' || c == '@');
}

internal b32 char_is_space(char c) {
  return c == ' ' || c == '\r' || c == '\t' || c == '\f' || c == '\v' || c == '\n';
}

internal char char_to_upper(char c) {
  return (c >= 'a' && c <= 'z') ? ('A' + (c - 'a')) : c;
}

internal char char_to_lower(char c) {
  return (c >= 'A' && c <= 'Z') ? ('a' + (c - 'A')) : c;
}