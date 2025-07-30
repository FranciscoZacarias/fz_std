#ifndef ERROR_H
#define ERROR_H

typedef enum
{
  Log_Level_Info = 0,
  Log_Level_Warning,
  Log_Level_Error,
  Log_Level_Fatal,

  Log_Level_Count
} Log_Level;

typedef struct Log_Entry Log_Entry;
struct Log_Entry
{
  Log_Level level;
  String8 message;

  String8     file;
  u32         line;
  OS_Date_Time timestamp;
};

#define MAX_ERROR_NODES 128
typedef struct Log_Entry_Node Log_Entry_Node;
struct Log_Entry_Node
{
  Log_Entry_Node* next;
  Log_Entry value;
};

typedef struct Log_Context Log_Context;
struct Log_Context
{
  Arena* arena;
  Log_Entry_Node* log_entry_first;
  Log_Entry_Node* log_entry_last;
  u32 log_count;
  String8 log_file_path;
};

global Log_Context g_log_context = {0};

function void log_init(String8 error_log_path);
function void log_emit(Log_Level level, String8 message, String8 file, u32 line);

#define emit_info(str8_msg)  log_emit(Log_Level_Info,   str8_msg, S(__FILE__), __LINE__)
#define emit_warn(str8_msg)  log_emit(Log_Level_Warning,str8_msg, S(__FILE__), __LINE__)
#define emit_error(str8_msg) log_emit(Log_Level_Error,  str8_msg, S(__FILE__), __LINE__)
#define emit_fatal(str8_msg) log_emit(Log_Level_Fatal,  str8_msg, S(__FILE__), __LINE__)

#endif // ERROR_H