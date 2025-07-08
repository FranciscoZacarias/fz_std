#ifndef FZ_INCLUDES_H
#define FZ_INCLUDES_H

//~ CLib
#include <stdlib.h>
#include <string.h>
#include <math.h> // TODO(fz): Replace/Implement these functions -> sinf(), cosf(), tan(), atan2f(), sqrtf(), fmaxf(), fabsf()

//~ Extern
#define STB_SPRINTF_STATIC
#define STB_SPRINTFZ_IMPLEMENTATION
#include "external/stb_sprintf.h"
#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include "external/stb_truetype.h"
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

//~ Single headers
#include "fz_core.h"
#include "fz_io.h" // TODO(fz): This is actually OS dependent 

//~ Headers
#include "fz_math.h"
#include "fz_memory.h"
#include "fz_string.h"
#include "fz_thread_context.h"

//~ Opengl specific headers
#include "glad/glad.h"
#include "glad/glad.c"
#include "extra/fz_opengl_helper.h"

//~ Window application
#include "fz_input.h"

#include "fz_win32.h"
#include "fz_win32_window.h"


//~ Source
#include "fz_math.c"
#include "fz_memory.c"
#include "fz_string.c"
#include "fz_thread_context.c"

//~ Opengl specific implementation
#include "extra/fz_opengl_helper.c"

//~ Window application
# include "fz_input.c"

#include "fz_win32.c"
#include "fz_win32_window.c"

#endif // FZ_INCLUDES_H