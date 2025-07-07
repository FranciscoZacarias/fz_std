#ifndef FZ_OPENGL_HELPER_H
#define FZ_OPENGL_HELPER_H

#define OPENGL_MAX_SHADER_STAGES 6

internal GLuint opengl_compile_program(String8 source_path, GLenum kind);
internal void   opengl_delete_program(GLuint program);
  
internal void opengl_set_uniform_mat4fv(u32 program, const char8* uniform, Mat4f32 mat);
internal void opengl_set_uniform_u32(u32 program, const char8* uniform, u32 value);
internal void opengl_set_uniform_f32(u32 program, const char8* uniform, f32 value);

internal void opengl_enable_debug_messages();
internal s32  opengl_get_max_textures();

// DOC(fz): Opengl hello world program from https://gist.github.com/mmozeiko/ed2ad27f75edf9c26053ce332a1f6647
// Requires:
// win32_init();
// win32_enable_window(width, height);
// win32_enable_opengl();
// win32_show_window(true);
internal void opengl_hello_world(HWND window, HDC device_context);
internal void opengl_fatal_error(char* message, ...);

// TODO(fz): Clean these up

internal void opengl_load_fallback_texture(GLuint* texture) {
  u32 pixels[] = {
    0x80000000, 0xffffffff,
    0xffffffff, 0x80000000,
  };

  glCreateTextures(GL_TEXTURE_2D, 1, texture);
  glTextureParameteri(*texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTextureParameteri(*texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTextureParameteri(*texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTextureParameteri(*texture, GL_TEXTURE_WRAP_T, GL_REPEAT);

  s32 width = 2;
  s32 height = 2;
  glTextureStorage2D(*texture, 1, GL_RGBA8, width, height);
  glTextureSubImage2D(*texture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

internal void opengl_check_error(const char* context) {
  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    const char* err_str = "UNKNOWN";
    switch (err) {
      case GL_INVALID_ENUM:                  err_str = "GL_INVALID_ENUM"; break;
      case GL_INVALID_VALUE:                 err_str = "GL_INVALID_VALUE"; break;
      case GL_INVALID_OPERATION:             err_str = "GL_INVALID_OPERATION"; break;
      case GL_INVALID_FRAMEBUFFER_OPERATION: err_str = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
      case GL_OUT_OF_MEMORY:                 err_str = "GL_OUT_OF_MEMORY"; break;
      case GL_STACK_UNDERFLOW:               err_str = "GL_STACK_UNDERFLOW"; break;
      case GL_STACK_OVERFLOW:                err_str = "GL_STACK_OVERFLOW"; break;
    }
    opengl_fatal_error("OpenGL error in %s: %s (0x%X)", context, err_str, err);
  }
}

internal void opengl_print_context() {
  printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
  printf("GL_RENDERER: %s\n", glGetString(GL_RENDERER));
  printf("GL_VENDOR: %s\n", glGetString(GL_VENDOR));
  
  GLint flags = 0;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  printf("Context Flags: 0x%x\n", flags);

  GLint profile = 0;
  glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile);
  printf("Context Profile: 0x%x\n", profile);
}

internal void opengl_dump_vertex_attributes() {
  for (GLint i = 0; i < 8; ++i) {
    GLint enabled = 0;
    glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);
    printf("layout (location = %d) %s\n", i, enabled ? "Enabled" : "Disabled");
  }
}

#endif // FZ_OPENGL_HELPER_H