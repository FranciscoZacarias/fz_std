// 
// Help:
// 
// Enables:
//   Create window:  #define FZ_ENABLE_WINDOW  1
//   Enable opengl:  #define FZ_ENABLE_OPENGL  1
//   Attach console: #define FZ_ENABLE_CONSOLE 1
//   Enable assert:  #define FZ_ENABLE_ASSERT  1
// 
// Config:
//   #define CAMERA_SENSITIVITY 1.0f
//   #define CAMERA_SPEED 8.0f
// 
// Macros that provide Context:
// {
//   COMPILER_CLANG
//   COMPILER_MSVC
//   COMPILER_GCC
// 
//   OS_WINDOWS
//   OS_LINUX
//   OS_MAC
// 
//   ARCH_X64
//   ARCH_X86
//   ARCH_ARM64
//   ARCH_ARM32
// 
//   COMPILER_MSVC_YEAR
// 
//   ARCH_32BIT
//   ARCH_64BIT
//   ARCH_LITTLE_ENDIAN
// }
// 

// Toggle between examples
#if 0
# define EXAMPLE_CONSOLE 1
#elif 0
# define EXAMPLE_WINDOW 1
#elif 0
# define EXAMPLE_WINDOW_AND_CONSOLE 1
#elif 1
# define EXAMPLE_WINDOW_OPENGL 1
#endif


////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
//~ CONSOLE APPLICATION EXAMPLE
#ifdef EXAMPLE_CONSOLE

// CODE BEGIN
#define FZ_ENABLE_CONSOLE 1
#include "fz_include.h"

// Run once at start of program
void application_init() {
  String a = StringLiteral("We are not your kind");
  println_string(a);
}

// Run every tick. Return false to exit.
void application_tick() {
  println_string(StringLiteral("Press any key to exit..."));
  getch();
  IsApplicationRunning = false;
}

#endif // CONSOLE_APP_EXAMPLE 


////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
//~ WINDOWS APPLICATION EXAMPLE
#ifdef EXAMPLE_WINDOW 

// CODE BEGIN
#define FZ_ENABLE_WINDOW 1
#include "fz_include.h"

// Run once at start of program
void application_init() {
  for (s32 i = 0; i < 500; i += 1) {
    win32_put_pixel(i, i, RGB(255, 0, 0));
  }
}

// Run every tick
void application_tick() {
  // TODO(Fz): We should also allocate a back buffer for this example.
  // It's very slow to put pixel by pixel.
  // Especially if we want to clear the screen every frame.
}

#endif // EXAMPLE_WINDOW 

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
//~ WINDOWS APPLICATION EXAMPLE
#ifdef EXAMPLE_WINDOW_AND_CONSOLE 

// CODE BEGIN
#define FZ_ENABLE_WINDOW 1
#define FZ_ENABLE_CONSOLE 1
#include "fz_include.h"

// Run once at start of program
void application_init() {
  for (s32 i = 0; i < 500; i += 1) {
    win32_put_pixel(i, i, RGB(255, 0, 0));
  }
  String a = StringLiteral("A line was drawn in the window!");
  println_string(a);
}

// Run every tick
void application_tick() {
  if (input_is_key_pressed(KeyboardKey_ESCAPE)) {
    IsApplicationRunning = false;
  }
  local_persist s32 offset = 1;
  if (input_is_key_pressed(KeyboardKey_A)) {
    String a = StringLiteral("A was pressed");
    for (s32 i = 0; i < 500; i += 1) {
      win32_put_pixel(i + offset, i, RGB(255, 0, 0));
    }
    offset += 1;
  }
  if (input_is_key_down(KeyboardKey_D)) {
    String a = StringLiteral("D is down");
    println_string(a);
    for (s32 i = 0; i < 500; i += 1) {
      win32_put_pixel(i + offset, i, RGB(255, 0, 0));
    }
    offset += 1;
  }
}

#endif // EXAMPLE_WINDOW_AND_CONSOLE 

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
//~ WINDOWS APPLICATION EXAMPLE
#ifdef EXAMPLE_WINDOW_OPENGL 

// CODE BEGIN
#define FZ_ENABLE_WINDOW 1
#define FZ_ENABLE_OPENGL 1

#include "fz_include.h"

global GLuint Vao, Vbo;
global OGL_Shader ShaderProgram;

// Run once at start of program
void application_init() {
  // Create shaders
  OGL_Shader vertexShader = ogl_make_shader(StringLiteral("vertex_shader.glsl"), GL_VERTEX_SHADER);
  OGL_Shader fragmentShader = ogl_make_shader(StringLiteral("fragment_shader.glsl"), GL_FRAGMENT_SHADER);

  // Create program
  GLuint shaders[] = { vertexShader, fragmentShader };
  ShaderProgram = ogl_make_program(shaders, 2);

  f32 vertices[] = {
    // positions        // colors
    0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, // bottom right
   -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, // bottom left
    0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f  // top 
  };

  glGenVertexArrays(1, &Vao);
  glGenBuffers(1, &Vbo);
  glBindVertexArray(Vao);

  glBindBuffer(GL_ARRAY_BUFFER, Vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(f32), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(f32), (void*)(3 * sizeof(f32)));
  glEnableVertexAttribArray(1);

  glUseProgram(ShaderProgram);
}

// Run every tick
void application_tick() {
  if (input_is_key_pressed(KeyboardKey_ESCAPE)) {
    IsApplicationRunning = false;
  }

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // render the triangle
  glBindVertexArray(Vao);
  glDrawArrays(GL_TRIANGLES, 0, 3);

  SwapBuffers(_DeviceContextHandle);
}

#endif // EXAMPLE_WINDOW_OPENGL 