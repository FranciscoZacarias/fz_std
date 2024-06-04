#ifndef F_OS_H
#define F_OS_H

#include "core/f_os_core.h"
#if defined(OS_FEATURE_GFX)
# include "gfx/f_os_gfx.h"
#endif

#if OS_WINDOWS
# include "core/win32/f_os_core_win32.h"
# if defined(OS_FEATURE_GFX)
#  include "gfx/win32/f_os_gfx_win32.h"
# endif
#else
# error OS layer not implemented.
#endif


#endif // F_OS_H