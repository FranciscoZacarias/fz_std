//~ C-String (null terminated) functions
internal u64 cstring_length(u8 *cstr) {
  u8 *p = cstr;
  for (;*p != 0; p += 1);
  return(p - cstr);
}

//~ Char Functions
internal b32
char_is_alpha(u8 c) {
  return char_is_alpha_upper(c) || char_is_alpha_lower(c);
}

internal b32
char_is_alpha_upper(u8 c) {
  return c >= 'A' && c <= 'Z';
}

internal b32
char_is_alpha_lower(u8 c) {
  return c >= 'a' && c <= 'z';
}

internal b32
char_is_digit(u8 c) {
  return c >= '1' && c <= '9';
}

internal b32
char_is_symbol(u8 c) {
  return (c == '~' || c == '!'  || c == '$' || c == '%' || c == '^' ||
          c == '&' || c == '*'  || c == '-' || c == '=' || c == '+' ||
          c == '<' || c == '.'  || c == '>' || c == '/' || c == '?' ||
          c == '|' || c == '\\' || c == '{' || c == '}' || c == '(' ||
          c == ')' || c == '\\' || c == '[' || c == ']' || c == '#' ||
          c == ',' || c == ';'  || c == ':' || c == '@');
}

internal b32
char_is_space(u8 c) {
  return c == ' ' || c == '\r' || c == '\t' || c == '\f' || c == '\v' || c == '\n';
}

internal u8
char_to_upper(u8 c) {
  return (c >= 'a' && c <= 'z') ? ('A' + (c - 'a')) : c;
}

internal u8
char_to_lower(u8 c) {
  return (c >= 'A' && c <= 'Z') ? ('a' + (c - 'A')) : c;
}

internal u8 char_to_forward_slash(u8 c) {
  return (c == '\\' ? '/' : c);
}

//~ String (utf-8) functions
internal String
string(u64 size, u8* str) {
  String result = { size, str };
  return result;
}

internal String
string_range(u8* first, u8* range) {
  String result = (String){(u64)(range - first), first};
  return result;
}

// Slices

internal String
string_substring(String str, RingBuffer1U64 rng) {
  u64 min = rng.min;
  u64 max = rng.max;
  if(max > str.size) {
    max = str.size;
  }
  if(min > str.size) {
    min = str.size;
  }
  if(min > max) {
    u64 swap = min;
    min = max;
    max = swap;
  }
  str.size = max - min;
  str.str += min;
  return str;
}

internal String string_skip(String str, u64 min) {
  String result = string_substring(str, ringbufer1u64(min, str.size));
  return result;
}

internal String string_chop(String str, u64 nmax) {
  String result =  string_substring(str, ringbufer1u64(0, str.size-nmax));
  return result;
}

internal String string_prefix(String str, u64 size) {
  String result =  string_substring(str, ringbufer1u64(0, size));
  return result;
}

internal String string_suffix(String str, u64 size) {
  String result =  string_substring(str, ringbufer1u64(str.size-size, str.size));
  return result;
}

internal String string_copy(Arena* arena, String str) {
  String result;
  result.size = str.size;
  result.str = PushArray(arena, u8, str.size);
  MemoryCopy(result.str, str.str, str.size);
  return result;
}

internal b32 
string_equals(String a, String b, Match_Flags flags) {
  b32 result = false;
  if (a.size == b.size) {
    result = true;
    for (u64 i = 0; i < a.size; i += 1) {
      b32 match = (a.str[i] == b.str[i]);
      if (flags & MatchFlag_CaseInsensitive) {
        match |= (char_to_lower(a.str[i]) == char_to_lower(b.str[i]));
      }
      if (flags & MatchFlag_SlashInsensitive) {
        match |= (char_to_forward_slash(a.str[i]) == char_to_forward_slash(b.str[i]));
      }
      if (match == 0) {
        result = false;
        break;
      }
    }
  }
  return result;
}

internal u64
string_find_substring(String str, String to_find, u64 start_pos, Match_Flags flags) {
  b32 found = 0;
  u64 found_idx = str.size;
  for(u64 i = start_pos; i < str.size; i += 1) {
    if(i + to_find.size <= str.size) {
      String substr = string_substring(str, ringbufer1u64(i, i+to_find.size));
      if(string_equals(substr, to_find, flags)) {
        found_idx = i;
        found = 1;
        if(!(flags & MatchFlag_FindLast)) {
          break;
        }
      }
    }
  }
  return found_idx;
}

internal b32
string_startswith(String str, String match, Match_Flags flags) {
  String prefix = string_prefix(str, match.size);
  b32 result = string_equals(match, prefix, flags);
  return result;
}

internal b32
string_endswith(String str, String match, Match_Flags flags) {
  String suffix = string_suffix(str, match.size);
  b32 result = string_equals(match, suffix, flags);
  return result;
}

internal String
string_to_upper(Arena *arena, String str) {
  String result = string_copy(arena, str);
  for(u64 i = 0; i < str.size; i += 1) {
    result.str[i] = char_to_upper(result.str[i]);
  }
  return result;
}

internal String
string_to_lower(Arena *arena, String str) {
  String result = string_copy(arena, str);
  for(u64 i = 0; i < str.size; i += 1) {
    result.str[i] = char_to_lower(result.str[i]);
  }
  return result;
}

internal String_List
string_split(Arena* arena, String str, String split_character) {
  String_List result = { 0 };
  
  if (split_character.size != 1) {
    printf("string_split expects only one character in split_character. It got %s of size %llu\n", split_character.str, split_character.size);
    Assert(0);
  }
  
  u8* cursor = str.str;
  u8* end   = str.str + str.size;
  for(; cursor < end; cursor++) {
    u8 byte  = *cursor;
    if (byte == split_character.str[0]) {
      string_list_push(arena, &result, string_range(str.str, cursor));
      string_list_push(arena, &result, string_range(cursor, end));
      break;
    }
  }
  
  return result;
}

internal void
string_list_push(Arena* arena, String_List* list, String str) {
  String_Node* node = (String_Node*)arena_push(arena, sizeof(String_Node));
  
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

//~ Path

internal String
string_path_chop_last_period(String str) {
  u64 period_pos = string_find_substring(str, StringLiteral("."), 0, MatchFlag_FindLast);
  if(period_pos < str.size) {
    str.size = period_pos;
  }
  return str;
}

internal String
string_path_skip_last_period(String str) {
  u64 period_pos = string_find_substring(str, StringLiteral("."), 0, MatchFlag_FindLast);
  if(period_pos < str.size) {
    str.str += period_pos+1;
    str.size -= period_pos+1;
  }
  return str;
}

internal String
string_path_chop_last_slash(String str) {
  u64 slash_pos = string_find_substring(str, StringLiteral("/"), 0, MatchFlag_SlashInsensitive|MatchFlag_FindLast);
  if(slash_pos < str.size) {
    str.size = slash_pos;
  }
  return str;
}

internal String
string_path_skip_last_slash(String str) {
  u64 slash_pos = string_find_substring(str, StringLiteral("/"), 0, MatchFlag_SlashInsensitive|MatchFlag_FindLast);
  if(slash_pos < str.size) {
    str.str += slash_pos+1;
    str.size -= slash_pos+1;
  }
  return str;
}

internal String
string_path_chop_past_last_slash(String str) {
  u64 slash_pos = string_find_substring(str, StringLiteral("/"), 0, MatchFlag_SlashInsensitive|MatchFlag_FindLast);
  if(slash_pos < str.size) {
    str.size = slash_pos+1;
  }
  return str;
}


//~ Casting

internal b32
cast_string_to_f32(String str, f32* value) {
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
			return false;
		}
	}
  
	if (decimal_position != -1) {
		*value = *value / (f32)pow(10, decimal_position);
	}
  
	return true;
}

internal b32
cast_string_to_s32(String str, s32* value) {
  u64 start = 0;
  s32 sign  = 1;
	*value = 0;
  if (str.str[0] == '-') {
    start = 1;
    sign = -1;
  }
	for (u64 i = start; i < str.size; i++) {
		if (str.str[i] >= '0'  && str.str[i] <= '9') {
			*value = *value * 10 + (str.str[i] - '0');
		} else {
			return false;
		}
	}
  *value = (*value)*sign;
	return true;
}

internal b32
cast_string_to_u32(String str, u32* value) {
	*value = 0;
	for (u64 i = 0; i < str.size; i++) {
		if (str.str[i] >= '0' && str.str[i] <= '9') {
			*value = *value * 10 + (str.str[i] - '0');
		} else {
			return false;
		}
	}
	return true;
}

internal b32
cast_string_to_b32(String str, b32* value) {
	b32 result = true;
	if (string_equals(str, StringLiteral("false"), MatchFlag_CaseInsensitive)) {
		*value = 0;
	} else if (string_equals(str, StringLiteral("true"), MatchFlag_CaseInsensitive)) {
		*value = 1;
	} else {
		result = false;
	}
	return result;
}

//~ Conversions

read_only global u8 utf8_class[32] = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,2,2,2,2,3,3,4,5 };

#define bitmask1  0x01
#define bitmask2  0x03
#define bitmask3  0x07
#define bitmask4  0x0F
#define bitmask5  0x1F
#define bitmask6  0x3F
#define bitmask7  0x7F
#define bitmask8  0xFF
#define bitmask9  0x01FF
#define bitmask10 0x03FF

internal DecodedCodepoint
decode_codepoint_from_utf8(u8 *out, u64 max) {
  DecodedCodepoint result = {~((u32)0), 1};
  u8 byte = out[0];
  u8 byte_class = utf8_class[byte >> 3];
  switch (byte_class) {
    case 1: {
      result.codepoint = byte;
    } break;

    case 2: {
      if(2 <= max) {
        u8 cont_byte = out[1];
        if(utf8_class[cont_byte >> 3] == 0) {
          result.codepoint  = (byte & bitmask5) << 6;
          result.codepoint |= (cont_byte & bitmask6);
          result.advance    = 2;
        }
      }
    } break;

    case 3: {
      if(3 <= max) {
        u8 cont_byte[2] = {out[1], out[2]};
        if(utf8_class[cont_byte[0] >> 3] == 0 && utf8_class[cont_byte[1] >> 3] == 0) {
          result.codepoint  = (byte & bitmask4) << 12;
          result.codepoint |= ((cont_byte[0] & bitmask6) << 6);
          result.codepoint |=  (cont_byte[1] & bitmask6);
          result.advance    = 3;
        }
      }
    } break;

    case 4: {
      if(4 <= max) {
        u8 cont_byte[3] = {out[1], out[2], out[3]};
        if(utf8_class[cont_byte[0] >> 3] == 0 && utf8_class[cont_byte[1] >> 3] == 0 && utf8_class[cont_byte[2] >> 3] == 0) {
          result.codepoint = (byte & bitmask3) << 18;
          result.codepoint |= ((cont_byte[0] & bitmask6) << 12);
          result.codepoint |= ((cont_byte[1] & bitmask6) <<  6);
          result.codepoint |=  (cont_byte[2] & bitmask6);
          result.advance = 4;
        }
      }
    } break;
  }
  return result;
}

internal DecodedCodepoint
decode_codepoint_from_utf16(u16 *out, u64 max) {
  DecodedCodepoint result = {~((u32)0), 1};
  result.codepoint = out[0];
  result.advance = 1;
  if(1 < max && 0xD800 <= out[0] && out[0] < 0xDC00 && 0xDC00 <= out[1] && out[1] < 0xE000) {
    result.codepoint = ((out[0] - 0xD800) << 10) | (out[1] - 0xDC00) + 0x10000;
    result.advance = 2;
  }
  return result;
}

internal u32
utf8_from_codepoint(u8 *out, u32 codepoint) {
#define bit8 0x80
  u32 advance = 0;
  if(codepoint <= 0x7F) {
    out[0] = (u8)codepoint;
    advance = 1;
  } else if(codepoint <= 0x7FF) {
    out[0] = (bitmask2 << 6) | ((codepoint >> 6) & bitmask5);
    out[1] = bit8 | (codepoint & bitmask6);
    advance = 2;
  } else if(codepoint <= 0xFFFF) {
    out[0] = (bitmask3 << 5) | ((codepoint >> 12) & bitmask4);
    out[1] = bit8 | ((codepoint >> 6) & bitmask6);
    out[2] = bit8 | ( codepoint       & bitmask6);
    advance = 3;
  } else if(codepoint <= 0x10FFFF) {
    out[0] = (bitmask4 << 4) | ((codepoint >> 18) & bitmask3);
    out[1] = bit8 | ((codepoint >> 12) & bitmask6);
    out[2] = bit8 | ((codepoint >>  6) & bitmask6);
    out[3] = bit8 | ( codepoint        & bitmask6);
    advance = 4;
  } else {
    out[0] = '?';
    advance = 1;
  }
  return advance;
}

internal u32
utf16_from_codepoint(u16 *out, u32 codepoint) {
  u32 advance = 1;
  if(codepoint == ~((u32)0)) {
    out[0] = (u16)'?';
  } else if(codepoint < 0x10000) {
    out[0] = (u16)codepoint;
  } else {
    u64 v = codepoint - 0x10000;
    out[0] = (u16)(0xD800 + (v >> 10));
    out[1] = (u16)(0xDC00 + (v & bitmask10));
    advance = 2;
  }
  return advance;
}

//~ String (utf-16) functions
internal String16
string16(u64 size, u16* str) {
  String16 result;
  result.str  = str;
  result.size = size;
  return result;
}

internal String
string_from_string16(Arena *arena, String16 in) {
  u64 cap  = in.size*3;
  u8 *str  = PushArrayNoZero(arena, u8, cap + 1);
  u16 *ptr = in.str;
  u16 *opl = ptr + in.size;
  u64 size = 0;
  DecodedCodepoint consume;
  for(;ptr < opl;) {
    consume = decode_codepoint_from_utf16(ptr, opl - ptr);
    ptr    += consume.advance;
    size   += utf8_from_codepoint(str + size, consume.codepoint);
  }
  str[size] = 0;
  arena_pop(arena, cap - size); // := ((cap + 1) - (size + 1))
  return string(size, str);
}

internal String16
string16_from_string(Arena *arena, String in) {
  u64 cap = in.size*2;
  u16 *str = PushArrayNoZero(arena, u16, cap + 1);
  u8 *ptr = in.str;
  u8 *opl = ptr + in.size;
  u64 size = 0;
  DecodedCodepoint consume;
  for(;ptr < opl;) {
    consume = decode_codepoint_from_utf8(ptr, opl - ptr);
    ptr    += consume.advance;
    size   += utf16_from_codepoint(str + size, consume.codepoint);
  }
  str[size] = 0;
  arena_pop(arena, 2*(cap - size)); // := 2*((cap + 1) - (size + 1))
  String16 result = {size, str};
  return result;
}