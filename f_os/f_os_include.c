#include "core/f_os_core.c"
#if defined(OS_FEATURE_GFX)
# include "gfx/f_os_gfx.c"
#endif

#if OS_WINDOWS
# include "core/win32/f_os_core_win32.c"
# if defined(OS_FEATURE_GFX)
#  include "gfx/win32/f_os_gfx_win32.c"
# endif
#else
# error OS layer not implemented.
#endif