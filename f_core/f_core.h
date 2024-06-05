#ifndef F_CORE_H
#define F_CORE_H

////////////////////////////////
// Context Cracking

/* 
  Macro quick look-up:

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
*/

// Clang OS/Arch Cracking
#if defined(__clang__)

# define COMPILER_CLANG 1
# if defined(_WIN32)
#  define OS_WINDOWS 1
# elif defined(__gnu_linux__) || defined(__linux__)
#  define OS_LINUX 1
# elif defined(__APPLE__) && defined(__MACH__)
#  define OS_MAC 1
# else
#  error This compiler/OS combo is not supported.
# endif

# if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
#  define ARCH_X64 1
# elif defined(i386) || defined(__i386) || defined(__i386__)
#  define ARCH_X86 1
# elif defined(__aarch64__)
#  define ARCH_ARM64 1
# elif defined(__arm__)
#  define ARCH_ARM32 1
# else
#  error Architecture not supported.
# endif

// MSVC OS/Arch Cracking
#elif defined(_MSC_VER)

# define COMPILER_MSVC 1
# if _MSC_VER >= 1930
#  define COMPILER_MSVC_YEAR 2022
# elif _MSC_VER >= 1920
#  define COMPILER_MSVC_YEAR 2019
# elif _MSC_VER >= 1910
#  define COMPILER_MSVC_YEAR 2017
# elif _MSC_VER >= 1900
#  define COMPILER_MSVC_YEAR 2015
# elif _MSC_VER >= 1800
#  define COMPILER_MSVC_YEAR 2013
# elif _MSC_VER >= 1700
#  define COMPILER_MSVC_YEAR 2012
# elif _MSC_VER >= 1600
#  define COMPILER_MSVC_YEAR 2010
# elif _MSC_VER >= 1500
#  define COMPILER_MSVC_YEAR 2008
# elif _MSC_VER >= 1400
#  define COMPILER_MSVC_YEAR 2005
# else
#  define COMPILER_MSVC_YEAR 0
# endif

# if defined(_WIN32)
#  define OS_WINDOWS 1
# else
#  error This compiler/OS combo is not supported.
# endif

# if defined(_M_AMD64)
#  define ARCH_X64 1
# elif defined(_M_IX86)
#  define ARCH_X86 1
# elif defined(_M_ARM64)
#  define ARCH_ARM64 1
# elif defined(_M_ARM)
#  define ARCH_ARM32 1
# else
#  error Architecture not supported.
# endif

// GCC OS/Arch Cracking
#elif defined(__GNUC__) || defined(__GNUG__)

# define COMPILER_GCC 1
# if defined(__gnu_linux__) || defined(__linux__)
#  define OS_LINUX 1
# else
#  error This compiler/OS combo is not supported.
# endif

# if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
#  define ARCH_X64 1
# elif defined(i386) || defined(__i386) || defined(__i386__)
#  define ARCH_X86 1
# elif defined(__aarch64__)
#  define ARCH_ARM64 1
# elif defined(__arm__)
#  define ARCH_ARM32 1
# else
#  error Architecture not supported.
# endif

#else
# error Compiler not supported.
#endif

// Arch Cracking
#if defined(ARCH_X64)
# define ARCH_64BIT 1
#elif defined(ARCH_X86)
# define ARCH_32BIT 1
#endif

#if ARCH_ARM32 || ARCH_ARM64 || ARCH_X64 || ARCH_X86
# define ARCH_LITTLE_ENDIAN 1
#else
# error Endianness of this architecture not understood by context cracker.
#endif

//~ Zero All Undefined Options
// Build options
#if !defined(IS_COMMAND_LINE_PROGRAM)
#define IS_COMMAND_LINE_PROGRAM 0
#endif
// Architecture
#if !defined(ARCH_32BIT)
# define ARCH_32BIT 0
#endif
#if !defined(ARCH_64BIT)
# define ARCH_64BIT 0
#endif
#if !defined(ARCH_X64)
# define ARCH_X64 0
#endif
#if !defined(ARCH_X86)
# define ARCH_X86 0
#endif
#if !defined(ARCH_ARM64)
# define ARCH_ARM64 0
#endif
#if !defined(ARCH_ARM32)
# define ARCH_ARM32 0
#endif
// Compiler
#if !defined(COMPILER_MSVC)
# define COMPILER_MSVC 0
#endif
#if !defined(COMPILER_GCC)
# define COMPILER_GCC 0
#endif
#if !defined(COMPILER_CLANG)
# define COMPILER_CLANG 0
#endif
// Operating system
#if !defined(OS_WINDOWS)
# define OS_WINDOWS 0
#endif
#if !defined(OS_LINUX)
# define OS_LINUX 0
#endif
#if !defined(OS_MAC)
# define OS_MAC 0
#endif
#if !defined(ASAN_ENABLED)
# define ASAN_ENABLED 0
#endif

#if ASAN_ENABLED && COMPILER_MSVC
# define no_asan __declspec(no_sanitize_address)
#elif ASAN_ENABLED && (COMPILER_CLANG || COMPILER_GCC)
# define no_asan __attribute__((no_sanitize("address")))
#endif
#if !defined(no_asan)
# define no_asan
#endif

#if COMPILER_MSVC
# define thread_static __declspec(thread)
#elif COMPILER_CLANG || COMPILER_GCC
# define thread_static __thread
#endif

#if OS_WINDOWS
# define shared_function C_LINKAGE __declspec(dllexport)
#else
# define shared_function C_LINKAGE
#endif

#if OS_WINDOWS
# pragma section(".roglob", read)
# define read_only no_asan __declspec(allocate(".roglob"))
#else
# define read_only
#endif

#if COMPILER_MSVC
# define per_thread __declspec(thread)
#elif COMPILER_CLANG
# define per_thread __thread
#elif COMPILER_GCC
# define per_thread __thread
#endif

#if COMPILER_MSVC && COMPILER_MSVC_YEAR < 2015
# define inline_internal static
#else
# define inline_internal inline static
#endif


#if LANG_CPP
# define C_LINKAGE_BEGIN extern "C"{
# define C_LINKAGE_END }
# define C_LINKAGE extern "C"
#else
# define C_LINKAGE_BEGIN
# define C_LINKAGE_END
# define C_LINKAGE
#endif

////////////////////////////////
// Core

#define true  1
#define false 0

#define Stringify_(S) #S
#define Stringify(S) Stringify_(S)
#define Glue_(A,B) A##B
#define Glue(A,B) Glue_(A,B)

#if COMPILER_MSVC
# define Trap() __debugbreak()
#elif COMPILER_CLANG || COMPILER_GCC
# define Trap() __builtin_trap()
#else
# error Unknown trap intrinsic for this compiler.
#endif

#define Statement(S) do{ S }while(0)

#if !defined(AssertBreak)
# define AssertBreak() (*(volatile int*)0 = 0)
#endif

#if ENABLE_ASSERT
# define Assert(c) Statement( if (!(c)){ AssertBreak(); } )
# define AssertNoReentry() Statement(local_persist b32 triggered = 0;Assert(triggered == 0); triggered = 1;) 
#else
# define Assert(c)
# define AssertNoReentry()
#endif

#define StaticAssert(c,l) typedef u8 Glue(l,__LINE__) [(c)?1:-1]

#define ArrayCount(a) (sizeof(a)/sizeof((a)[0]))

#define Min(A,B) (((A)<(B))?(A):(B))
#define Max(A,B) (((A)>(B))?(A):(B))
#define ClampTop(A,X) Min(A,X)
#define ClampBot(X,B) Max(X,B)
#define Clamp(min,val,max) (((val)<(min))?(min):((val)>(max))?(max):(val))

#define IntFromPtr(p) (u64)((u8*)p - (u8*)0)
#define PtrFromInt(i) (void*)((u8*)0 + (i))
#define Member(T,m) (((T*)0)->m)
#define OffsetOfMember(T,m) IntFromPtr(&Member(T,m))

#define Kilobytes(n) ((u64)(n * 1024llu))
#define Megabytes(n) ((u64)(n * 1024llu * 1024llu))
#define Gigabytes(n) ((u64)(n * 1024llu * 1024llu * 1024llu))
#define Terabytes(n) ((u64)(n * 1024llu * 1024llu * 1024llu * 1024llu))

#define Thousand(n) ((n)*1000)
#define Million(n)  ((n)*1000000llu)
#define Billion(n)  ((n)*1000000000llu)
#define Trillion(n) ((n)*1000000000000llu)

#define DEFAULT_ALIGNMENT sizeof(void*)
#define AlignPow2(x,b)     (((x) + (b) - 1)&(~((b) - 1)))
#define AlignDownPow2(x,b) ((x)&(~((b) - 1)))
#define IsPow2(x)          ((x)!=0 && ((x)&((x)-1))==0)
#define IsPow2OrZero(x)    ((((x) - 1)&(x)) == 0)

#define Swap(type, a, b) do{ type _swapper_ = a; a = b; b = _swapper_; }while(0)

#define AbsoluteValueS32(x) (s32)abs((x))
#define AbsoluteValueS64(x) (s64)llabs((s64)(x))
#define AbsoluteValueU32(x) (u32)abs((x))
#define AbsoluteValueU64(x) (u64)llabs((u64)(x))

//~ Linked list helpers

#define CheckNull(p) ((p)==0)
#define SetNull(p) ((p)=0)

#define QueuePush_NZ(f,l,n,next,zchk,zset) (zchk(f)?\
(((f)=(l)=(n)), zset((n)->next)):\
((l)->next=(n),(l)=(n),zset((n)->next)))
#define QueuePushFront_NZ(f,l,n,next,zchk,zset) (zchk(f) ? (((f) = (l) = (n)), zset((n)->next)) :\
((n)->next = (f)), ((f) = (n)))
#define QueuePop_NZ(f,l,next,zset) ((f)==(l)?\
(zset(f),zset(l)):\
((f)=(f)->next))
#define StackPush_N(f,n,next) ((n)->next=(f),(f)=(n))
#define StackPop_NZ(f,next,zchk) (zchk(f)?0:((f)=(f)->next))

#define DLLInsert_NPZ(f,l,p,n,next,prev,zchk,zset) \
(zchk(f) ? (((f) = (l) = (n)), zset((n)->next), zset((n)->prev)) :\
zchk(p) ? (zset((n)->prev), (n)->next = (f), (zchk(f) ? (0) : ((f)->prev = (n))), (f) = (n)) :\
((zchk((p)->next) ? (0) : (((p)->next->prev) = (n))), (n)->next = (p)->next, (n)->prev = (p), (p)->next = (n),\
((p) == (l) ? (l) = (n) : (0))))
#define DLLPushBack_NPZ(f,l,n,next,prev,zchk,zset) DLLInsert_NPZ(f,l,l,n,next,prev,zchk,zset)
#define DLLRemove_NPZ(f,l,n,next,prev,zchk,zset) (((f)==(n))?\
((f)=(f)->next, (zchk(f) ? (zset(l)) : zset((f)->prev))):\
((l)==(n))?\
((l)=(l)->prev, (zchk(l) ? (zset(f)) : zset((l)->next))):\
((zchk((n)->next) ? (0) : ((n)->next->prev=(n)->prev)),\
(zchk((n)->prev) ? (0) : ((n)->prev->next=(n)->next))))

#define QueuePush(f,l,n)         QueuePush_NZ(f,l,n,next,CheckNull,SetNull)
#define QueuePushFront(f,l,n)    QueuePushFront_NZ(f,l,n,next,CheckNull,SetNull)
#define QueuePop(f,l)            QueuePop_NZ(f,l,next,SetNull)
#define StackPush(f,n)           StackPush_N(f,n,next)
#define StackPop(f)              StackPop_NZ(f,next,CheckNull)
#define DLLPushBack(f,l,n)       DLLPushBack_NPZ(f,l,n,next,prev,CheckNull,SetNull)
#define DLLPushFront(f,l,n)      DLLPushBack_NPZ(l,f,n,prev,next,CheckNull,SetNull)
#define DLLInsert(f,l,p,n)       DLLInsert_NPZ(f,l,p,n,next,prev,CheckNull,SetNull)
#define DLLRemove(f,l,n)         DLLRemove_NPZ(f,l,n,next,prev,CheckNull,SetNull)

//~ Defer

#define DeferLoop(start, end) for(int _i_ = ((start), 0); _i_ == 0; _i_ += 1, (end))
#define DeferLoopChecked(begin, end) for(int _i_ = 2 * !(begin); (_i_ == 2 ? ((end), 0) : !_i_); _i_ += 1, (end))

//~ Memory

#define MemoryCopy(dest,value,size)  memmove((dest),(value),(size))
#define MemoryCopyStruct(dest,value) MemoryCopy((dest),(value), Min(sizeof(*(dest)), sizeof(*(size))))
#define MemoryZero(s,z)            memset((s),0,(z))
#define MemoryZeroStruct(s)        MemoryZero((s),sizeof(*(s)))
#define MemorySet(dest,value,size) memset((dest),(value),(size))
#define MemoryMatch(a,b,z)        (memcmp((a),(b),(z)) == 0)

#define local_persist   static
#define global          static
#define internal        static
#define fallthrough

////////////////////////////////
// Types 

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;
#define U8_MAX  0xFF
#define U8_MIN  0x00
#define U16_MAX 0xFFFF
#define U16_MIN 0x0000
#define U32_MAX 0xFFFFFFFFu
#define U32_MIN 0x00000000u
#define U64_MAX 0xFFFFFFFFFFFFFFFFull
#define U64_MIN 0x0000000000000000ull

typedef signed char      s8;
typedef signed short     s16;
typedef signed int       s32;
typedef signed long long s64;
#define S8_MAX  0x7F
#define S8_MIN  (-S8_MAX - 1)
#define S16_MAX 0x7FFF
#define S16_MIN (-S16_MAX - 1)
#define S32_MAX 0x7FFFFFFF
#define S32_MIN (-S32_MAX - 1)
#define S64_MAX 0x7FFFFFFFFFFFFFFFll
#define S64_MIN (-S64_MAX - 1ll)

typedef float  f32;
typedef double f64;
#define F32_MAX 0x1.fffffep+127f
#define F32_MIN (-F32_MAX)
#define F64_MAX 0x1.fffffffffffffp+1023
#define F64_MIN (-F64_MAX)

typedef s8  b8;
typedef s32 b32;

typedef struct DateTime {
  u16 year;
  u8 month;         // [1,  12]
  u8 day_of_week;   // [0,   6]
  u8 day;           // [1,  31]
  u8 hour;          // [0,  23]
  u8 minute;        // [0,  59]
  u8 second;        // [0,  59]
  u16 milliseconds; // [0, 999]
} DateTime;

inline_internal b32
date_time_match(DateTime a, DateTime b) {
  return (a.year == b.year &&
          a.month == b.month &&
          a.day_of_week == b.day_of_week &&
          a.day == b.day &&
          a.hour == b.hour &&
          a.minute == b.minute &&
          a.second == b.second &&
          a.milliseconds == b.milliseconds);
}

inline_internal b32
date_time_less_than(DateTime a, DateTime b) {
  b32 result = 0;
  if (0){}
  else if (a.year < b.year) { result = 1; }
  else if (a.year > b.year) { result = 0; }
  else if (a.month < b.month) { result = 1; }
  else if (a.month > b.month) { result = 0; }
  else if (a.day < b.day) { result = 1; }
  else if (a.day > b.day) { result = 0; }
  else if (a.hour < b.hour) { result = 1; }
  else if (a.hour > b.hour) { result = 0; }
  else if (a.minute < b.minute) { result = 1; }
  else if (a.minute > b.minute) { result = 0; }
  else if (a.second < b.second) { result = 1; }
  else if (a.second > b.second) { result = 0; }
  else if (a.milliseconds < b.milliseconds) { result = 1; }
  else if (a.milliseconds > b.milliseconds) { result = 0; }
  return result;
}

typedef enum Axis2 {
  Axis2_Invalid = -1,
  Axis2_X,
  Axis2_Y,
  Axis2_COUNT
} Axis2;

typedef enum Axis3 {
  Axis3_Invalid = -1,
  Axis3_X,
  Axis3_Y,
  Axis3_Z,
  Axis3_COUNT
} Axis3;

typedef enum Axis4 {
  Axis4_Invalid = -1,
  Axis4_X,
  Axis4_Y,
  Axis4_Z,
  Axis4_W,
  Axis4_COUNT
} Axis4;

#endif // F_CORE_H