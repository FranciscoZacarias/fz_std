#ifndef F_STRING_H
#define F_STRING_H

typedef u32 Match_Flags;
enum {
  MatchFlag_CaseInsensitive  = (1<<0),
  MatchFlag_RightSideSloppy  = (1<<1),
  MatchFlag_SlashInsensitive = (1<<2),
  MatchFlag_FindLast         = (1<<3),
  MatchFlag_KeepEmpties      = (1<<4),
};

typedef struct DecodedCodepoint {
  u32 codepoint;
  u32 advance;
} DecodedCodepoint;

typedef struct String {
  u64 size;
  u8* str;
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

typedef struct String16 {
  u64  size;
  u16* str;
} String16;

//~ C-String (null terminated) functions
internal u64 cstring_length(u8 *cstr);

//~ Char Functions
internal b32 char_is_alpha(u8 c);
internal b32 char_is_alpha_upper(u8 c);
internal b32 char_is_alpha_lower(u8 c);
internal b32 char_is_digit(u8 c);
internal b32 char_is_symbol(u8 c);
internal b32 char_is_space(u8 c);
internal u8  char_to_upper(u8 c);
internal u8  char_to_lower(u8 c);
internal u8  char_to_forward_slash(u8 c);

//~ String (utf-8) functions
#define StringLiteral(s) (String){sizeof(s)-1, (u8*)(s)}
internal String string(u64 size, u8* str);
internal String string_range(u8* first, u8* range);
internal String string_copy(Arena* arena, String str);
internal String string_to_upper(Arena *arena, String str);
internal String string_to_lower(Arena *arena, String str);

// Slices
internal String string_substring(String str, RingBuffer1U64 rng);
internal String string_skip(String str, u64 min);
internal String string_chop(String str, u64 nmax);
internal String string_prefix(String str, u64 size);
internal String string_suffix(String str, u64 size);

// Match
internal b32 string_equals(String a, String b, Match_Flags flags);
internal u64 string_find_substring(String str, String to_find, u64 start_pos, Match_Flags flags);
internal b32 string_startswith(String str, String match, Match_Flags flags);
internal b32 string_endswith(String str, String match, Match_Flags flags);

internal String_List string_split(Arena* arena, String str, String split_character);
internal void        string_list_push(Arena* arena, String_List* list, String str);

// Path
internal String string_path_chop_last_period(String str);
internal String string_path_skip_last_period(String str);
internal String string_path_chop_last_slash(String str);
internal String string_path_skip_last_slash(String str);
internal String string_path_chop_past_last_slash(String str);

// Casting
internal b32 cast_string_to_f32(String str, f32* value);
internal b32 cast_string_to_s32(String str, s32* value);
internal b32 cast_string_to_u32(String str, u32* value);
internal b32 cast_string_to_b32(String str, b32* value);

//~ Conversions
internal DecodedCodepoint decode_codepoint_from_utf8(u8 *out, u64 max);
internal DecodedCodepoint decode_codepoint_from_utf16(u16 *out, u64 max);
internal u32 utf8_from_codepoint(u8 *out, u32 codepoint);
internal u32 utf16_from_codepoint(u16 *out, u32 codepoint);

//~ String (utf-16) functions
internal String16 string16(u64 size, u16* str);
internal String   string_from_string16(Arena *arena, String16 in);
internal String16 string16_from_string(Arena *arena, String in);

#endif // F_STRING_H