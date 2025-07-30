#ifndef WIN32_OPENGL_H
#define WIN32_OPENGL_H

#include "win32_wgl_constants.h"

///////////////////////////////////////////////////////
// @Section: Define wgl function pointers
#define WGL_FUNC(ret,name,params) typedef ret (* PFN##name##PROC) params;
# include "win32_wgl_functions.inl"
#undef WGL_FUNC

///////////////////////////////////////////////////////
// @Section: Generate wgl definitions
#define WGL_FUNC(ret,name,params) global PFN##name##PROC name = NULL;
# include "win32_wgl_functions.inl"
#undef WGL_FUNC

///////////////////////////////////////////////////////
// @Section: Functions
function b32 _win32_load_wgl_functions(); /* Opens webgl functions */

#endif // WIN32_OPENGL_H