#ifndef F_OS_GFX_H
#define F_OS_GFX_H

#include "f_os_gfx_tables.h"

typedef enum OS_EventKind {
  OS_EventKind_Null,
  OS_EventKind_WindowClose,
  OS_EventKind_WindowLoseFocus,
  OS_EventKind_Press,
  OS_EventKind_Release,
  OS_EventKind_Text,
  OS_EventKind_MouseScroll,
  OS_EventKind_DropFile,
  OS_EventKind_COUNT
} OS_EventKind;

typedef u32 OS_Modifiers;
enum {
  OS_Modifier_Ctrl  = (1<<0),
  OS_Modifier_Shift = (1<<1),
  OS_Modifier_Alt   = (1<<2),
};

typedef struct OS_Event OS_Event;
struct OS_Event {
  OS_Event *next;
  OS_Event *prev;
  OS_Handle window;
  OS_EventKind kind;
  OS_Modifiers modifiers;
  OS_Key key;
  u32 character;
  Vec2f32 position;
  Vec2f32 scroll;
  String path;
};

typedef struct OS_EventList OS_EventList;
struct OS_EventList {
  OS_Event *first;
  OS_Event *last;
  u64 count;
};

internal void os_init_gfx();

//~ Accessors
internal Vec2f32 os_mouse_from_window(OS_Handle handle);

#endif // F_OS_GFX_H
