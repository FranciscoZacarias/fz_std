#ifndef BASE_H
#define BASE_H

/*
NOTE(fz):
Must be in every project:
  #include "fz_base.h"
  - This includes the base library as well as the core os module.
  - The base layer should have no dependencies, apart from the core os layer, which obviously requires the underlying OS implementations.
  - Enables window API
  - Only supports 1 window
  #include "fz_opengl.h"

Macros:
  - DEBUG
  - OS_WINDOWS, OS_LINUX, OS_MAC
  - COMPILER_CLANG, COMPILER_GCC, COMPILER_MSVC, COMPILER_MSVC_YEAR, 
  - ARCH_X64, ARCH_X86, ARCH_ARM64, ARCH_ARM32, ARCH_ADDRSIZE, ARCH_LITTLE_ENDIAN
*/

#pragma warning(disable: 4201) // NOTE(fz): nonstandard extension used: nameless struct/union

#define FZ_BASE 1

///////////////////////////////////////////////////////
// @Section: System includes
#include <stdio.h>
#include <math.h>

///////////////////////////////////////////////////////
// @Section: *.h
#include "base\context_cracking.h"
#include "base\core.h" 
#include "base\memory.h"
#include "base\math.h"
#include "base\art.h"
#include "base\string.h"
#include "base\thread_context.h"
#include "base\command_line.h"
#include "base\entry_point.h"
#include "os\os.h"
#include "base\error.h"

///////////////////////////////////////////////////////
// @Section: *.c
#include "base\memory.c"
#include "base\math.c"
#include "base\art.c"
#include "base\string.c"
#include "base\thread_context.c"
#include "base\command_line.c"
#include "base\entry_point.c"
#include "os\os.c"
#include "base\error.c"

#endif // BASE_H