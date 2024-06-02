#include "f_os/core/f_os_core.c"

#if OS_WINDOWS
# include "f_os/core/win32/f_os_core_win32.c"
#else
# error OS layer not implemented.
#endif