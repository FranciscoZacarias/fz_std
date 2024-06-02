#ifndef F_OS_H
#define F_OS_H

#include "f_os/core/f_os_core.h"

#if OS_WINDOWS
# include "f_os/core/win32/f_os_core_win32.h"
#else
# error OS layer not implemented.
#endif


#endif // F_OS_H