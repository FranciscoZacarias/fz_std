#ifndef FZ_OPENGL_HELPER_H
#define FZ_OPENGL_HELPER_H

typedef GLuint OGL_Shader;

internal OGL_Shader opengl_make_shader(String8 shader_path, GLenum kind);
internal OGL_Shader opengl_make_program(GLuint *shaders, u32 count);
internal void       opengl_delete_shader(OGL_Shader shader);
  
internal void opengl_set_uniform_mat4fv(u32 program, const char8* uniform, Mat4f32 mat);
internal void opengl_set_uniform_u32(u32 program, const char8* uniform, u32 value);
internal void opengl_set_uniform_f32(u32 program, const char8* uniform, f32 value);

internal s32 opengl_get_max_textures();

#endif // FZ_OPENGL_HELPER_H