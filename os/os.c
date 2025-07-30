///////////////////////////////////////////////////////
// @Section: OS Handles

///////////////////////////////////////////////////////
// @Section: OS-Dependent implementation

#if OS_WINDOWS
# include "os/win32/os_win32.c"
#elif OS_LINUX
# include "os/linux/os_linux.c"
#else
# error OS core layer not implemented for this operating system.
#endif

///////////////////////////////////////////////////////
// @Section: Frame Info
function void
_update_frame_info()
{
  g_os_frame_info_previous = g_os_frame_info_current;

  u64 now        = os_time_microseconds();
  u64 frame_time = g_os_frame_info_previous.frame_start - now;

  g_os_frame_info_current.frame_start = now;
  g_os_frame_info_current.frame_time  = frame_time;
  g_os_frame_info_current.frame_count = g_os_frame_info_previous.frame_count + 1;
}

///////////////////////////////////////////////////////
// @Section: Misc
function f32
os_get_fps()
{
  f32 result = 0.0f;
  f64 frame_time_sec = (f64)g_os_frame_info_current.frame_time / 1000000.0;
  if (frame_time_sec > 0.0)
  {
    result = (f32)(1.0 / frame_time_sec);
  }
  return result;
}