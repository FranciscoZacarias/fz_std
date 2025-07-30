#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

// The acceptable commands are:
// --command "this is my value" -no-string
// key: "command", value: "this is my value", is-flag: false
// -no-string
// key: "no-string", value: "no-string", is-flag: true

#define MAX_COMMAND_LINE_ARGS 16
#define TEMP_BUFFER_SIZE 2048
#define PARSED_BUFFER_SIZE 4096

typedef struct Command_Line_Arg Command_Line_Arg;
struct Command_Line_Arg {
  b32     is_flag;
  String8 key;
  String8 value;
};

function Command_Line_Arg command_line_arg_new(String8 key, String8 value, b32 is_flag);

typedef struct Command_Line Command_Line;
struct Command_Line 
{
  String8 executable;
  String8 raw_args;
  Command_Line_Arg args[MAX_COMMAND_LINE_ARGS];
  u32              args_count;
};

function Command_Line command_line_parse(String8 input);
function Command_Line command_line_parse_from_argc_argv(s32 argc, u8** argv);
function String8      command_line_parse_token(u8** cursor);
function void         command_line_skip_whitespace(u8** cursor);
function String8      command_line_strip_quotes(String8 in);
function String8      command_line_strip_leading_dashes(String8 in);

#endif // COMMAND_LINE_H