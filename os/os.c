///////////////////////////////////////////////////////
// @Section: Input
function void
_input_init(OS_Window* window, Input_State* input)
{
  AssertNoReentry();
  MemoryZeroStruct(input);

  input->mouse_current.screen_space.x = window->dimensions.x/2;
  input->mouse_current.screen_space.y = window->dimensions.y/2;
  
  input->mouse_previous.screen_space.x = window->dimensions.x/2;
  input->mouse_previous.screen_space.y = window->dimensions.y/2;

  input->_g_ignore_next_mouse_move = false;
  input->_g_is_cursor_locked       = false;
}

function void
_input_update(Input_State* input)
{
  MemoryCopy(&(input->keyboard_previous), &(input->keyboard_current), sizeof(Keyboard_State));
  MemoryCopy(&(input->mouse_previous),    &(input->mouse_current),    sizeof(Mouse_State));
}

function b32
input_is_key_up(Input_State* input, Keyboard_Key key)
{
  b32 result = input->keyboard_current.keys[key] == false;
  return result;
}

function b32
input_is_key_down(Input_State* input, Keyboard_Key key)
{
  b32 result = input->keyboard_current.keys[key] == true;
  return result;
}

function b32
input_was_key_up(Input_State* input, Keyboard_Key key)
{
  b32 result = input->keyboard_previous.keys[key] == false;
  return result;
}

function b32
input_was_key_down(Input_State* input, Keyboard_Key key)
{
  b32 result = input->keyboard_previous.keys[key] == true;
  return result;
}

function b32
input_is_key_pressed(Input_State* input, Keyboard_Key key)
{
  return input_is_key_down(input, key) && input_was_key_up(input, key);
}

function void
_input_process_keyboard_key(Input_State* input, Keyboard_Key key, b8 is_pressed)
{
  if (input->keyboard_current.keys[key] != is_pressed)
  {
    input->keyboard_current.keys[key] = is_pressed;
  }
}

function b32
input_is_button_up(Input_State* input, Mouse_Button button)
{
  b32 result = input->mouse_current.buttons[button] == false;
  return result;
}

function b32
input_is_button_down(Input_State* input, Mouse_Button button)
{
  b32 result = input->mouse_current.buttons[button] == true;
  return result;
}

function b32
input_was_button_up(Input_State* input, Mouse_Button button)
{
  b32 result = input->mouse_previous.buttons[button] == false;
  return result;
}

function b32
input_was_button_down(Input_State* input, Mouse_Button button)
{
  b32 result = input->mouse_previous.buttons[button] == true;
  return result;
}

function b32
input_is_button_pressed(Input_State* input, Mouse_Button button)
{
  b32 result = input_is_button_down(input, button) && input_was_button_up(input, button);
  return result;
}

function void
_input_process_mouse_button(Input_State* input, Mouse_Button button, b32 is_pressed)
{
  if (input->mouse_current.buttons[button] != (b8)is_pressed)
  {
    input->mouse_current.buttons[button] = (b8)is_pressed;
  }
}

function void
_input_process_mouse_cursor(Input_State* input, s32 x, s32 y)
{
  // Copy current state to previous
  MemoryCopyStruct(&(input->mouse_previous), &(input->mouse_current));

  // Compute new deltas
  input->mouse_current.delta.x = x - input->mouse_previous.screen_space.x;
  input->mouse_current.delta.y = y - input->mouse_previous.screen_space.y;

  // Update current position
  input->mouse_current.screen_space.x = x;
  input->mouse_current.screen_space.y = y;
}

///////////////////////////////////////////////////////
// @Section: OS-Dependent implementation

#if OS_WINDOWS
# include "os/win32/os_win32.c"
#elif OS_LINUX
# include "os/linux/os_linux.c"
#else
# error OS core layer not implemented for this operating system.
#endif