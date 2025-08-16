#ifndef STRING_H
#define STRING_H

typedef struct String8_Node String8_Node;
typedef struct String8 String8;
typedef struct String8_List String8_List;

// 8 Bit strings
struct String8 {
  u64 size;
  u8* str;
};
#define S(s) (String8){sizeof(s)-1, (u8*)(s)}
#define Sf(arena,fmt,...) string8_from_format(arena, fmt, __VA_ARGS__)

struct String8_Node {
  struct String8_Node* next;
  String8 value;
};

struct String8_List {
  String8_Node* first;
  String8_Node* last;
  u64 node_count;
  u64 total_size;
};

function String8 string8_new(u64 size, u8* str); /* Create a new String8 with given size and data pointer. */
function String8 string8_copy(Arena* arena, String8 source); /* Allocate and copy source string into arena. */
function String8 string8_range(u8* first, u8* range); /* Create String8 from first pointer to range pointer (exclusive). */
function String8 string8_concat(Arena* arena, String8 a, String8 b); /* Allocate concatenated string a+b in arena. */
function String8 string8_slice(String8 str, u64 start, u64 end); /* Extract substring from start to end (exclusive). */
function String8 string8_trim(String8 str); /* Remove leading and trailing whitespace. */
function b32     string8_contains(String8 str, String8 substring); /* Check if str contains substring. */
function b32     string8_find_first(String8 str, String8 substring, u64* index); /* Find first occurrence of substring, write index. */
function b32     string8_find_last(String8 str, String8 substring, u64* index); /* Find last occurrence of substring, write index. */
function b32     string8_match(String8 a, String8 b, b32 case_sensitive); /* Compare strings for equality with case sensitivity option. */
function String8 string8_from_format(Arena* arena, char const* fmt, ...); /* Printf-style string formatting into arena. */

function String8_List string8_split(Arena* arena, String8 str, String8 split_character); /* Split string by delimiter into list. */
function String8_List string8_list_new(Arena* arena, String8 str); /* Create new list with single string element. */
function String8      string8_list_pop(String8_List* list); /* Remove and return first element from list. */
function void         string8_list_clear(String8_List* list); /* Clear all elements from list. */
function void         string8_list_push(Arena* arena, String8_List* list, String8 str); /* Add string to end of list. */
function String8      string8_list_join(Arena* arena, String8_List* list); /* Concatenate all list elements into single string. */

function b32      f32_from_string8(String8 str, f32* value); /* Parse float from string, return success. */
function b32      s32_from_string8(String8 str, s32* value); /* Parse signed int from string, return success. */
function b32      b32_from_string8(String8 str, b32* value); /* Parse boolean from string, return success. */
function u8*      cstring_from_string8(Arena* arena, String8 str); /* Convert to null-terminated C string in arena. */
function String8  string8_from_cstring(u8* cstring); /* Create String8 from null-terminated C string. */
function u32      cstring_length(u8* cstring); /* Get length of null-terminated C string. */

function b32 char8_is_alpha(u8 c); /* Check if character is alphabetic. */
function b32 char8_is_alphanum(u8 c); /* Check if character is alphanumeric. */
function b32 char8_is_alpha_upper(u8 c); /* Check if character is uppercase letter. */
function b32 char8_is_alpha_lower(u8 c); /* Check if character is lowercase letter. */
function b32 char8_is_digit(u8 c); /* Check if character is digit 0-9. */
function b32 char8_is_symbol(u8 c); /* Check if character is symbol/punctuation. */
function b32 char8_is_space(u8 c); /* Check if character is whitespace. */
function u8  char8_to_upper(u8 c); /* Convert character to uppercase. */
function u8  char8_to_lower(u8 c); /* Convert character to lowercase. */

#endif // STRING_H