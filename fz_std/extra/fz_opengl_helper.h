#ifndef FZ_OPENGL_HELPER_H
#define FZ_OPENGL_HELPER_H

typedef GLuint OGL_Shader;

internal OGL_Shader ogl_make_shader(String shader_path, GLenum kind);
internal OGL_Shader ogl_make_program(GLuint *shaders, u32 count);

internal void renderer_set_uniform_mat4fv(u32 program, const char* uniform, Mat4f32 mat);

#endif // FZ_OPENGL_HELPER_H