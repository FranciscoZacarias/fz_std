#ifndef F_INCLUDES_H
#define F_INCLUDES_H

/*
  Enable opengl:
  #define F_ENABLE_OPENGL 1
*/

//~ CLib
#include <stdlib.h>
#include <stdio.h> // TODO(fz): Move this to a f_stdio, so we can properly abstract into our String API
#include <string.h>
#include <math.h> // TODO(fz): Replace/Implement these functions -> sinf(), cosf(), tan(), atan2f(), sqrtf(), floor(), fminf(), fmaxf(), fabsf()

//~ Extern
#include "external/stb_sprintf.h"

//~ Single headers
#include "f_core.h"

//~ Headers
#include "f_math.h"
#include "f_memory.h"
#include "f_string.h"
#include "f_thread_context.h"
#include "win32/f_win32.h"

#if defined(F_ENABLE_OPENGL)
# include "f_opengl_helper.h"
#endif 

//~ Extern
#define STB_SPRINTF_IMPLEMENTATION
#include "external/stb_sprintf.h"

//~ Source
#include "f_math.c"
#include "f_memory.c"
#include "f_string.c"
#include "f_thread_context.c"
#include "win32/f_win32.c"

#if defined(F_ENABLE_OPENGL)
# include "f_opengl_helper.c"
#endif

#endif // F_INCLUDES_H