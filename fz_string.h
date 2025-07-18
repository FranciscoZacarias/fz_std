#ifndef FZ_STRING_H
#define FZ_STRING_H

// 8 Bit strings
typedef struct String8 {
  u64  size;
  char8* str;
} String8;
#define Str8(s) (String8){sizeof(s)-1, (char8*)(s)}

typedef struct String8_Node {
  struct String8_Node* next;
  String8 value;
} String8_Node;

typedef struct String8_List {
  String8_Node* first;
  String8_Node* last;
  u64 node_count;
  u64 total_size;
} String8_List;

internal String8 string8_new(u64 size, char8* str);
internal String8 string8_format(Arena* arena, String8 fmt, ...);
internal String8 string8_range(char8* first, char8* range);
internal String8 string8_concat(Arena* arena, String8 a, String8 b); // TODO(fz): Test this func
internal String8 string8_slice(String8 str, u64 start, u64 end);
internal String8 string8_trim(String8 str);
internal b32     string8_contains(String8 str, String8 substring);
internal b32     string8_find_first(String8 str, String8 substring, u64* index);
internal b32     string8_find_last(String8 str, String8 substring, u64* index); 
internal b32     string8_equal(String8 a, String8 b);
internal void    string8_printf(String8 str);

internal String8_List string8_split(Arena* arena, String8 str, String8 split_character);
internal String8_List string8_list_new(Arena* arena, String8 str);
internal String8      string8_list_pop(String8_List* list);
internal void         string8_list_clear(String8_List* list);
internal void         string8_list_push(Arena* arena, String8_List* list, String8 str);
internal String8      string8_list_join(Arena* arena, String8_List* list);

internal b32     f32_from_string8(String8 str, f32* value);
internal b32     s32_from_string8(String8 str, s32* value);
internal b32     b32_from_string8(String8 str, b32* value);
internal char8*  cstring_from_string8(Arena* arena, String8 str);
internal String8 string8_from_cstring(char8* cstring);

// 16 Bit strings
typedef struct String16 {
  u64 size;
  char16* str;
} String16;
#define Str16(s) (String16{sizeof(s)-1, (char16*)(s)};

internal String8  string8_from_string16(Arena* arena, String16 str16);
internal String16 string16_from_string8(Arena *arena, String8 str8);
internal wchar_t* wcstr_from_string16(Arena *arena, String16 str16); // TODO(fz): This is a windows only function.

//~ Char Functions
internal b32   char8_is_alpha(char8 c);
internal b32   char8_is_alphanum(char8 c);
internal b32   char8_is_alpha_upper(char8 c);
internal b32   char8_is_alpha_lower(char8 c);
internal b32   char8_is_digit(char8 c);
internal b32   char8_is_symbol(char8 c);
internal b32   char8_is_space(char8 c);
internal char8 char8_to_upper(char8 c);
internal char8 char8_to_lower(char8 c);

#endif // FZ_STRING_H