#ifndef STRING_H
#define STRING_H

// 8 Bit strings
typedef struct String8 String8;
struct String8 {
  u64 size;
  u8* str;
};
#define S(s) (String8){sizeof(s)-1, (u8*)(s)}

typedef struct String8_Node String8_Node;
struct String8_Node {
  struct String8_Node* next;
  String8 value;
};

typedef struct String8_List String8_List;
struct String8_List {
  String8_Node* first;
  String8_Node* last;
  u64 node_count;
  u64 total_size;
};

function String8 string8_new(u64 size, u8* str);
function String8 string8_copy(Arena* arena, String8 source);
function String8 string8_range(u8* first, u8* range);
function String8 string8_concat(Arena* arena, String8 a, String8 b); // TODO(fz): Test this func
function String8 string8_slice(String8 str, u64 start, u64 end);
function String8 string8_trim(String8 str);
function b32     string8_contains(String8 str, String8 substring);
function b32     string8_find_first(String8 str, String8 substring, u64* index);
function b32     string8_find_last(String8 str, String8 substring, u64* index); 
function b32     string8_match(String8 a, String8 b, b32 case_sensitive);
function void    string8_printf(String8 str);
function String8 string8_from_format(Arena* arena, char const* fmt, ...);

function String8_List string8_split(Arena* arena, String8 str, String8 split_character);
function String8_List string8_list_new(Arena* arena, String8 str);
function String8      string8_list_pop(String8_List* list);
function void         string8_list_clear(String8_List* list);
function void         string8_list_push(Arena* arena, String8_List* list, String8 str);
function String8      string8_list_join(Arena* arena, String8_List* list);

function b32      f32_from_string8(String8 str, f32* value);
function b32      s32_from_string8(String8 str, s32* value);
function b32      b32_from_string8(String8 str, b32* value);
function u8*      cstring_from_string8(Arena* arena, String8 str);
function String8  string8_from_cstring(u8* cstring);
function u32      cstring_length(u8* cstring);

//~ Char Functions
function b32 char8_is_alpha(u8 c);
function b32 char8_is_alphanum(u8 c);
function b32 char8_is_alpha_upper(u8 c);
function b32 char8_is_alpha_lower(u8 c);
function b32 char8_is_digit(u8 c);
function b32 char8_is_symbol(u8 c);
function b32 char8_is_space(u8 c);
function u8  char8_to_upper(u8 c);
function u8  char8_to_lower(u8 c);

#endif // STRING_H