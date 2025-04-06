#ifndef FZ_STRING_H
#define FZ_STRING_H

typedef struct String {
  u64  size;
  char* str;
} String;

typedef struct String_Node {
  struct String_Node* next;
  String value;
} String_Node;

typedef struct String_List {
  String_Node* first;
  String_Node* last;
  u64 node_count;
  u64 total_size;
} String_List;

//~ String Functions

#define StringLiteral(s) (String){sizeof(s)-1, (char*)(s)}
internal String string_new(u64 size, char* str);
internal String string_format(String fmt, ...);
internal String string_range(char* first, char* range);
internal String string_concat(Arena* arena, String a, String b);
internal String string_trim(String str);
internal b32    string_contains(String str, String substring);
internal s64    string_find_first(String str, String substring);
internal s64    string_find_last(String str, String substring); 
internal b32    string_equal(String a, String b);

internal String_List string_split(Arena* arena, String str, String split_character);
internal String      string_list_pop(String_List* list);
internal void        string_list_clear(String_List* list);
internal void        string_list_push(Arena* arena, String_List* list, String str);

internal b32 cast_string_to_f32(String str, f32* value);
internal b32 cast_string_to_s32(String str, s32* value);
internal b32 cast_string_to_b32(String str, b32* value);

//~ Char Functions

internal b32 char_is_alpha(char c);
internal b32 char_is_alpha_upper(char c);
internal b32 char_is_alpha_lower(char c);
internal b32 char_is_digit(char c);
internal b32 char_is_symbol(char c);
internal b32 char_is_space(char c);
internal char char_to_upper(char c);
internal char char_to_lower(char c);

#endif // FZ_STRING_H