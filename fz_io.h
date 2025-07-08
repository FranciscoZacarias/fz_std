#include <stdio.h>

//~ Printf colors
typedef enum Terminal_Color {
  Terminal_Color_Default,
  Terminal_Color_Black,
  Terminal_Color_Red,
  Terminal_Color_Green,
  Terminal_Color_Yellow,
  Terminal_Color_Blue,
  Terminal_Color_Magenta,
  Terminal_Color_Cyan,
  Terminal_Color_White,
  Terminal_Color_Gray,
  Terminal_Color_Bright_Red,
  Terminal_Color_Bright_Green,
  Terminal_Color_Bright_Yellow,
  Terminal_Color_Bright_Blue,
  Terminal_Color_Bright_Magenta,
  Terminal_Color_Bright_Cyan,
  Terminal_Color_Bright_White,
} Terminal_Color;

void printf_color(Terminal_Color color, const char* fmt, ...) {
  local_persist const char* ansi_colors[] = {
    "\x1b[0m",  // Default
    "\x1b[30m", // Black
    "\x1b[31m", // Red
    "\x1b[32m", // Green
    "\x1b[33m", // Yellow
    "\x1b[34m", // Blue
    "\x1b[35m", // Magenta
    "\x1b[36m", // Cyan
    "\x1b[37m", // White
    "\x1b[90m", // Gray
    "\x1b[91m", // Bright Red
    "\x1b[92m", // Bright Green
    "\x1b[93m", // Bright Yellow
    "\x1b[94m", // Bright Blue
    "\x1b[95m", // Bright Magenta
    "\x1b[96m", // Bright Cyan
    "\x1b[97m", // Bright White
  };

  if ((u32)color >= sizeof(ansi_colors)/sizeof(ansi_colors[0])) {
    color = Terminal_Color_Default;
  }

  printf("%s", ansi_colors[color]);

  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);

  printf("\x1b[0m"); // Reset to default
}
