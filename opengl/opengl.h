#ifndef OS_OPENGL_H
#define OS_OPENGL_H

#include "opengl_constants.h"

///////////////////////////////////////////////////////
// @Section: Define Opengl function pointers
typedef const GLubyte* (*PFNglGetStringPROC)(GLenum name);
typedef void (*GLDEBUGPROC)(GLenum source, GLenum type, GLuint id, GLenum severity,GLsizei length, const GLchar *message, const void *userParam);
#define GL_FUNC(ret,name,params) typedef ret (* PFN##name##PROC) params;
# include "opengl_functions.inl"
#undef GL_FUNC

///////////////////////////////////////////////////////
// @Section: Generate Opengl definitions
#define GL_FUNC(ret,name,params) global PFN##name##PROC name = NULL;
# include "opengl_functions.inl"
#undef GL_FUNC

///////////////////////////////////////////////////////
// @Section: Opengl entry point
function b32  os_opengl_init(); /* Initializes opengl context */
function void os_opengl_end();  /* Deletes opengl context */

///////////////////////////////////////////////////////
// @Section: Settings
function void os_window_enable_vsync(b32 state); /* Enables vsync */

///////////////////////////////////////////////////////
// @Section: Opengl helpers
function void  APIENTRY _os_opengl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *user); /* Opengl debug callback */
function void*          _load_gl_function(const char *name);                                                                                                        /* Helper to load a single opengl function */
function b32            _os_opengl_load_functions();                                                                                                                /* Loads opengl function pointers */

#endif // OS_OPENGL_H