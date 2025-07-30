function Command_Line_Arg
command_line_arg_new(String8 key, String8 value, b32 is_flag)
{
  Command_Line_Arg result = (Command_Line_Arg)
  {
    .is_flag = is_flag,
    .key     = key,
    .value   = value  
  };
  return result;
}

function void
command_line_skip_whitespace(u8** cursor)
{
  while (char8_is_space(**cursor)) (*cursor)++;
}

function String8
command_line_strip_quotes(String8 in)
{
  if (in.size >= 2 && in.str[0] == '"' && in.str[in.size - 1] == '"')
  {
    return string8_new(in.size - 2,  in.str + 1);
  }
  return in;
}

function String8
command_line_strip_leading_dashes(String8 in)
{
  u64 offset = 0;
  while (offset < in.size && in.str[offset] == '-')
  {
    offset++;
  }
  return string8_new(in.size - offset, in.str + offset);
}

function String8
command_line_parse_token(u8** cursor)
{
  command_line_skip_whitespace(cursor);
  if (**cursor == 0)
  {
    return (String8){0};
  }

  u8* start = *cursor;
  u8* end = start;

  if (*start == '"')
  {
    start++; // skip opening quote
    end = start;
    while (*end && *end != '"') end++;
    *cursor = (*end == '"') ? end + 1 : end;
  }
  else
  {
    while (*end && !char8_is_space(*end)) end++;
    *cursor = end;
  }

  String8 result = string8_new((u64)(end - start), start);
  return result;
}

function Command_Line
command_line_parse_from_argc_argv(s32 argc, u8** argv)
{
  Command_Line result = {0};
  Scratch scratch = scratch_begin(0,0);
  if (argc > 0)
  {
    String8 first_arg = string8_new(cstring_length(argv[0]), argv[0]);
    String8_List arg_list = string8_list_new(scratch.arena, first_arg);
    for (s32 idx = 1; idx < argc; idx += 1)
    {
      String8 arg = string8_new(cstring_length(argv[idx]), argv[idx]);
      string8_list_push(scratch.arena, &arg_list, arg);
    }
    String8 args = string8_list_join(scratch.arena, &arg_list);
    result = command_line_parse(args);
  }
  scratch_end(&scratch);
  return result;
}

function Command_Line
command_line_parse(String8 input)
{
  Command_Line result = {0};

  static u8 exe_buffer[MAX_PATH];
  DWORD exe_len = GetModuleFileNameA(0, exe_buffer, MAX_PATH);
  result.executable = (String8){ exe_len, exe_buffer };

  // Copy input into stable memory
  static u8 temp_buffer[TEMP_BUFFER_SIZE];
  static u8 parsed_buffer[PARSED_BUFFER_SIZE];
  u64 parsed_cursor = 0;

  u64 len = input.size;
  if (len >= sizeof(temp_buffer)) len = sizeof(temp_buffer) - 1;
  MemoryCopy(temp_buffer, input.str, len);
  temp_buffer[len] = 0;

  result.raw_args = (String8){ len, temp_buffer };

  u8* cursor = temp_buffer;

  while (*cursor && result.args_count < MAX_COMMAND_LINE_ARGS)
  {
    command_line_skip_whitespace(&cursor);
    if (*cursor == 0) break;

    String8 token = command_line_parse_token(&cursor);
    if (token.size == 0) break;

    if (token.str[0] == '-')
    {
      String8 key = command_line_strip_leading_dashes(token);

      // Copy key to stable memory
      String8 key_copy = {
        .size = key.size,
        .str = parsed_buffer + parsed_cursor
      };
      MemoryCopy(key_copy.str, key.str, key.size);
      parsed_cursor += key.size;

      // Peek for value
      command_line_skip_whitespace(&cursor);
      if (*cursor == 0 || *cursor == '-')
      {
        // Flag
        result.args[result.args_count++] = command_line_arg_new(key_copy, key_copy, true);
      }
      else
      {
        String8 val = command_line_parse_token(&cursor);
        val = command_line_strip_quotes(val);

        // Copy value to stable memory
        String8 val_copy = {
          .size = val.size,
          .str = parsed_buffer + parsed_cursor
        };
        MemoryCopy(val_copy.str, val.str, val.size);
        parsed_cursor += val.size;

        result.args[result.args_count++] = command_line_arg_new(key_copy, val_copy, false);
      }
    }
  }

  return result;
}