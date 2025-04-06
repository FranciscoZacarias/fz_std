#ifndef FZ_INCLUDES_H
#define FZ_INCLUDES_H

/*
  Help:

  Enables:
    Create window:  #define FZ_ENABLE_WINDOW  1
    Enable opengl:  #define FZ_ENABLE_OPENGL  1
    Attach console: #define FZ_ENABLE_CONSOLE 1
    Enable assert:  #define FZ_ENABLE_ASSERT  1
  
  Config:
    #define CAMERA_SENSITIVITY 1.0f
    #define CAMERA_SPEED 8.0f

  Macros that provide Context:
  {
    COMPILER_CLANG
    COMPILER_MSVC
    COMPILER_GCC

    OS_WINDOWS
    OS_LINUX
    OS_MAC

    ARCH_X64
    ARCH_X86
    ARCH_ARM64
    ARCH_ARM32

    COMPILER_MSVC_YEAR

    ARCH_32BIT
    ARCH_64BIT
    ARCH_LITTLE_ENDIAN
  }

*/

//~ CLib
#include <stdlib.h>
#include <string.h>
#include <math.h> // TODO(fz): Replace/Implement these functions -> sinf(), cosf(), tan(), atan2f(), sqrtf(), fmaxf(), fabsf()

//~ Extern
#include "external/stb_sprintf.h"

//~ Single headers
#include "fz_core.h"
#include "fz_io.h"

//~ Headers
#include "fz_math.h"
#include "fz_memory.h"
#include "fz_string.h"
#include "fz_thread_context.h"
#include "fz_input.h"
#include "fz_win32.h"
#include "fz_win32_window.h"

#if defined(FZ_ENABLE_OPENGL)
# include "extra/fz_opengl_helper.h"
#endif 

//~ Extern
#define STB_SPRINTFZ_IMPLEMENTATION
#include "external/stb_sprintf.h"

//~ Source
#include "fz_math.c"
#include "fz_memory.c"
#include "fz_string.c"
#include "fz_thread_context.c"
#include "fz_input.c"
#include "fz_win32.c"
#include "fz_win32_window.c"

#if defined(FZ_ENABLE_OPENGL)
# include "extra/fz_opengl_helper.c"
#endif

#endif // FZ_INCLUDES_H