///////////////////////////////////////////////////////
// @Section: Input
function void
_input_init()
{
  AssertNoReentry();
  MemoryZeroStruct(&_g_input_state);

  _g_input_state.mouse_current.screen_space.x = g_os_window.dimensions.x/2;
  _g_input_state.mouse_current.screen_space.y = g_os_window.dimensions.y/2;
  
  _g_input_state.mouse_previous.screen_space.x = g_os_window.dimensions.x/2;
  _g_input_state.mouse_previous.screen_space.y = g_os_window.dimensions.y/2;
}

function void
_input_update()
{
  MemoryCopy(&_g_input_state.keyboard_previous, &_g_input_state.keyboard_current, sizeof(Keyboard_State));
  MemoryCopy(&_g_input_state.mouse_previous,    &_g_input_state.mouse_current,    sizeof(Mouse_State));
}

function b32
input_is_key_up(Keyboard_Key key)
{
  b32 result = _g_input_state.keyboard_current.keys[key] == false;
  return result;
}

function b32
input_is_key_down(Keyboard_Key key)
{
  b32 result = _g_input_state.keyboard_current.keys[key] == true;
  return result;
}

function b32
input_was_key_up(Keyboard_Key key)
{
  b32 result = _g_input_state.keyboard_previous.keys[key] == false;
  return result;
}

function b32
input_was_key_down(Keyboard_Key key)
{
  b32 result = _g_input_state.keyboard_previous.keys[key] == true;
  return result;
}

function b32
input_is_key_pressed(Keyboard_Key key)
{
  return input_is_key_down(key) && input_was_key_up(key);
}

function void
_input_process_keyboard_key(Keyboard_Key key, b8 is_pressed)
{
  if (_g_input_state.keyboard_current.keys[key] != is_pressed)
  {
    _g_input_state.keyboard_current.keys[key] = is_pressed;
  }
}

function b32
input_is_button_up(Mouse_Button button)
{
  b32 result = _g_input_state.mouse_current.buttons[button] == false;
  return result;
}

function b32
input_is_button_down(Mouse_Button button)
{
  b32 result = _g_input_state.mouse_current.buttons[button] == true;
  return result;
}

function b32
input_was_button_up(Mouse_Button button)
{
  b32 result = _g_input_state.mouse_previous.buttons[button] == false;
  return result;
}

function b32
input_was_button_down(Mouse_Button button)
{
  b32 result = _g_input_state.mouse_previous.buttons[button] == true;
  return result;
}

function b32
input_is_button_pressed(Mouse_Button button)
{
  b32 result = input_is_button_down(button) && input_was_button_up(button);
  return result;
}

function void
_input_process_mouse_button(Mouse_Button button, b32 is_pressed)
{
  if (_g_input_state.mouse_current.buttons[button] != (b8)is_pressed)
  {
    _g_input_state.mouse_current.buttons[button] = (b8)is_pressed;
  }
}

function void
_input_process_mouse_cursor(s32 x, s32 y)
{
  // Copy current state to previous
  MemoryCopyStruct(&(_g_input_state.mouse_previous), &(_g_input_state.mouse_current));

  // Compute new deltas
  _g_input_state.mouse_current.delta.x = x - _g_input_state.mouse_previous.screen_space.x;
  _g_input_state.mouse_current.delta.y = y - _g_input_state.mouse_previous.screen_space.y;

  // Update current position
  _g_input_state.mouse_current.screen_space.x = x;
  _g_input_state.mouse_current.screen_space.y = y;
}
