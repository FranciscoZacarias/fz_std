internal GLuint opengl_compile_program(String8 source_path, GLenum shader_stage) {
  Arena_Temp scratch = scratch_begin(0, 0);
  GLuint program = 0;
    
  // Load shader source
  File_Data shader_source = file_load(scratch.arena, source_path);
  if (shader_source.data.size == 0) {
    ERROR_MESSAGE_AND_EXIT("Failed to load shader file: %.*s", (s32)source_path.size, source_path.str);
  }
    
  scratch_end(&scratch);
  return program;
}

internal GLuint opengl_create_pipeline(GLuint* programs, u32 count) {
  GLuint pipeline = 0;

  return pipeline;
}

internal void opengl_set_uniform_mat4fv(u32 program, const char8* uniform, Mat4f32 mat) {
  s32 uniform_location = glGetUniformLocation(program, uniform);
  if (uniform_location == -1) {
    printf("Mat4f32 :: Uniform %s not found for program %d\n", uniform, program);
  }
  glUniformMatrix4fv(uniform_location, 1, 1, &mat.data[0][0]);
}

internal void opengl_set_uniform_u32(u32 program, const char8* uniform, u32 value) {
  GLint uniform_location = glGetUniformLocation(program, uniform);
  if (uniform_location == -1) {
    printf("u32 :: Uniform %s not found for program %d\n", uniform, program);
  }
  glUniform1ui(uniform_location, value);
}

internal void opengl_set_uniform_f32(u32 program, const char8* uniform, f32 value) {
  GLint uniform_location = glGetUniformLocation(program, uniform);
  if (uniform_location == -1) {
    printf("f32 :: Uniform %s not found for program %d\n", uniform, program);
  }
  glUniform1f(uniform_location, value);
}

internal void opengl_delete_program(GLuint program) {
  glDeleteProgram(program);
}

internal s32 opengl_get_max_textures() {
  s32 max_gpu_textures;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_gpu_textures);
  return max_gpu_textures;
}

internal void opengl_fatal_error(char* message, ...) {
  va_list argptr;
  va_start(argptr, message);
  vfprintf(stderr, message, argptr);
  va_end(argptr);
  MessageBoxA(NULL, message, "Error", MB_ICONEXCLAMATION);
  ExitProcess(0);
}

internal void opengl_hello_world(HWND window, HDC device_context) {
  struct Vertex {
    float position[2];
    float uv[2];
    float color[3];
  };

  // vertex buffer containing triangle vertices
  GLuint vbo;
  {
    struct Vertex data[] = {
      { { -0.00f, +0.75f }, { 25.0f, 50.0f }, { 1, 0, 0 } },
      { { +0.75f, -0.50f }, {  0.0f,  0.0f }, { 0, 1, 0 } },
      { { -0.75f, -0.50f }, { 50.0f,  0.0f }, { 0, 0, 1 } },
    };
    glCreateBuffers(1, &vbo);
    glNamedBufferStorage(vbo, sizeof(data), data, 0);
  }

  // vertex input
  GLuint vao;
  {
    glCreateVertexArrays(1, &vao);

    GLint vbuf_index = 0;
    glVertexArrayVertexBuffer(vao, vbuf_index, vbo, 0, sizeof(struct Vertex));

    GLint a_pos = 0;
    glVertexArrayAttribFormat(vao, a_pos, 2, GL_FLOAT, GL_FALSE, offsetof(struct Vertex, position));
    glVertexArrayAttribBinding(vao, a_pos, vbuf_index);
    glEnableVertexArrayAttrib(vao, a_pos);

    GLint a_uv = 1;
    glVertexArrayAttribFormat(vao, a_uv, 2, GL_FLOAT, GL_FALSE, offsetof(struct Vertex, uv));
    glVertexArrayAttribBinding(vao, a_uv, vbuf_index);
    glEnableVertexArrayAttrib(vao, a_uv);

    GLint a_color = 2;
    glVertexArrayAttribFormat(vao, a_color, 3, GL_FLOAT, GL_FALSE, offsetof(struct Vertex, color));
    glVertexArrayAttribBinding(vao, a_color, vbuf_index);
    glEnableVertexArrayAttrib(vao, a_color);
  }

  // checkerboard texture, with 50% transparency on black colors
  GLuint texture;
  {
    unsigned int pixels[] =
    {
      0x80000000, 0xffffffff,
      0xffffffff, 0x80000000,
    };

    glCreateTextures(GL_TEXTURE_2D, 1, &texture);
    glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLsizei width = 2;
    GLsizei height = 2;
    glTextureStorage2D(texture, 1, GL_RGBA8, width, height);
    glTextureSubImage2D(texture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
  }

  // fragment & vertex shaders for drawing triangle
  GLuint pipeline, vshader, fshader;
  {
    const char* glsl_vshader =
      "#version 450 core                             \n"
      "                                              \n"
      "layout (location=0) in vec2 a_pos;            \n" // position attribute index 0
      "layout (location=1) in vec2 a_uv;             \n" // uv attribute index 1
      "layout (location=2) in vec3 a_color;          \n" // color attribute index 2
      "                                              \n"
      "layout (location=0)                           \n" // (from ARB_explicit_uniform_location)
      "uniform mat2 u_matrix;                        \n" // matrix uniform location 0
      "                                              \n"
      "out gl_PerVertex { vec4 gl_Position; };       \n" // required because of ARB_separate_shader_objects
      "out vec2 uv;                                  \n"
      "out vec4 color;                               \n"
      "                                              \n"
      "void main()                                   \n"
      "{                                             \n"
      "    vec2 pos = u_matrix * a_pos;              \n"
      "    gl_Position = vec4(pos, 0, 1);            \n"
      "    uv = a_uv;                                \n"
      "    color = vec4(a_color, 1);                 \n"
      "}                                             \n"
    ;

    const char* glsl_fshader =
      "#version 450 core                             \n"
      "                                              \n"
      "in vec2 uv;                                   \n"
      "in vec4 color;                                \n"
      "                                              \n"
      "layout (binding=0)                            \n" // (from ARB_shading_language_420pack)
      "uniform sampler2D s_texture;                  \n" // texture unit binding 0
      "                                              \n"
      "layout (location=0)                           \n"
      "out vec4 o_color;                             \n" // output fragment data location 0
      "                                              \n"
      "void main()                                   \n"
      "{                                             \n"
      "    o_color = color * texture(s_texture, uv); \n"
      "}                                             \n"
    ;

    vshader = glCreateShaderProgramv(GL_VERTEX_SHADER, 1, &glsl_vshader);
    fshader = glCreateShaderProgramv(GL_FRAGMENT_SHADER, 1, &glsl_fshader);

    GLint linked;
    glGetProgramiv(vshader, GL_LINK_STATUS, &linked);
    if (!linked)
    {
      char message[1024];
      glGetProgramInfoLog(vshader, sizeof(message), NULL, message);
      OutputDebugStringA(message);
      Assert(!"Failed to create vertex shader!");
    }

    glGetProgramiv(fshader, GL_LINK_STATUS, &linked);
    if (!linked)
    {
      char message[1024];
      glGetProgramInfoLog(fshader, sizeof(message), NULL, message);
      OutputDebugStringA(message);
      Assert(!"Failed to create fragment shader!");
    }

    glGenProgramPipelines(1, &pipeline);
    glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, vshader);
    glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT, fshader);
  }

  // setup global GL state
  {
    // enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // disble depth testing
    glDisable(GL_DEPTH_TEST);

    // disable culling
    glDisable(GL_CULL_FACE);
  }

  // show the window
  ShowWindow(window, SW_SHOWDEFAULT);

  LARGE_INTEGER freq, c1;
  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&c1);

  float angle = 0;

  for (;;)
  {
    // process all incoming Windows messages
    MSG msg;
    if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
    {
      if (msg.message == WM_QUIT)
      {
        break;
      }
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
      continue;
    }

    // get current window client area size
    RECT rect;
    GetClientRect(window, &rect);
    s32 width  = rect.right - rect.left;
    s32 height = rect.bottom - rect.top;

    LARGE_INTEGER c2;
    QueryPerformanceCounter(&c2);
    float delta = (float)((double)(c2.QuadPart - c1.QuadPart) / freq.QuadPart);
    c1 = c2;

    // render only if window size is non-zero
    if (width != 0 && height != 0)
    {
      // setup output size covering all client area of window
      glViewport(0, 0, width, height);

      // clear screen
      glClearColor(0.392f, 0.584f, 0.929f, 1.f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

      // setup rotation matrix in uniform
      {
        angle += delta * 2.0f * (float)PI / 20.0f; // full rotation in 20 seconds
        angle = fmodf(angle, 2.0f * (float)PI);

        float aspect = (float)height / width;
        float matrix[] =
        {
          cosf(angle) * aspect, -sinf(angle),
          sinf(angle) * aspect,  cosf(angle),
        };

        GLint u_matrix = 0;
        glProgramUniformMatrix2fv(vshader, u_matrix, 1, GL_FALSE, matrix);
      }

      // activate shaders for next draw call
      glBindProgramPipeline(pipeline);

      // provide vertex input
      glBindVertexArray(vao);

      // bind texture to texture unit
      GLint s_texture = 0; // texture unit that sampler2D will use in GLSL code
      glBindTextureUnit(s_texture, texture);

      // draw 3 vertices as triangle
      glDrawArrays(GL_TRIANGLES, 0, 3);

      // swap the buffers to show output
      if (!SwapBuffers(device_context))
      {
        opengl_fatal_error("Failed to swap OpenGL buffers!");
      }
    }
  }
}
