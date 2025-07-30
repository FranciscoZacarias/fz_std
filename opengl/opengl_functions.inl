// Shaders and Programs (SSO)
GL_FUNC(GLuint, glCreateShaderProgramv,  (GLenum type, GLsizei count, const char *const* strings))
GL_FUNC(void,   glProgramUniform1i,      (GLuint program, GLint location, GLint v0))
GL_FUNC(void,   glProgramUniform1f,      (GLuint program, GLint location, GLfloat v0))
GL_FUNC(void,   glProgramUniform2f,      (GLuint program, GLint location, GLfloat v0, GLfloat v1))
GL_FUNC(void,   glProgramUniform3f,      (GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2))
GL_FUNC(void,   glProgramUniform4f,      (GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3))
GL_FUNC(void,   glProgramUniformMatrix4fv, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value))
GL_FUNC(void,   glProgramUniformMatrix2fv, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value))

GL_FUNC(GLuint, glCreateProgram,         (void))
GL_FUNC(void,   glAttachShader,          (GLuint program, GLuint shader))
GL_FUNC(void,   glLinkProgram,           (GLuint program))
GL_FUNC(void,   glDeleteShader,          (GLuint shader))
GL_FUNC(void,   glDeleteProgram,         (GLuint program))
GL_FUNC(void,   glGetProgramiv,          (GLuint program, GLenum pname, GLint *params))
GL_FUNC(void,   glGetProgramInfoLog,     (GLuint program, GLsizei bufSize, GLsizei *length, char *infoLog))

// Pipeline (SSO)
GL_FUNC(void,   glGenProgramPipelines,   (GLsizei n, GLuint *pipelines))
GL_FUNC(void,   glBindProgramPipeline,   (GLuint pipeline))
GL_FUNC(void,   glUseProgramStages,      (GLuint pipeline, GLbitfield stages, GLuint program))
GL_FUNC(void,   glDeleteProgramPipelines,(GLsizei n, const GLuint *pipelines))

// Buffers (DSA)
GL_FUNC(void,   glCreateBuffers,         (GLsizei n, GLuint *buffers))
GL_FUNC(void,   glNamedBufferData,       (GLuint buffer, GLsizeiptr size, const void *data, GLenum usage))
GL_FUNC(void,   glNamedBufferSubData,    (GLuint buffer, GLintptr offset, GLsizeiptr size, const void *data))
GL_FUNC(void,   glDeleteBuffers,         (GLsizei n, const GLuint *buffers))
GL_FUNC(void,   glBindBuffer,            (GLenum target, GLuint buffer)) // still used in some cases
GL_FUNC(void,   glNamedBufferStorage,    (GLuint buffer, GLsizeiptr size, const void *data, GLbitfield flags))


// Vertex Arrays & Attributes (DSA)
GL_FUNC(void,   glCreateVertexArrays,    (GLsizei n, GLuint *arrays))
GL_FUNC(void,   glVertexArrayVertexBuffer, (GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride))
GL_FUNC(void,   glEnableVertexArrayAttrib, (GLuint vaobj, GLuint index))
GL_FUNC(void,   glVertexArrayAttribFormat, (GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset))
GL_FUNC(void,   glVertexArrayAttribBinding, (GLuint vaobj, GLuint attribindex, GLuint bindingindex))
GL_FUNC(void,   glBindVertexArray,       (GLuint array))
GL_FUNC(void,   glDeleteVertexArrays,    (GLsizei n, const GLuint *arrays))

// Textures (DSA)
GL_FUNC(void,   glCreateTextures,        (GLenum target, GLsizei n, GLuint *textures))
GL_FUNC(void,   glTextureStorage2D,      (GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height))
GL_FUNC(void,   glTextureSubImage2D,     (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels))
GL_FUNC(void,   glTextureParameteri,     (GLuint texture, GLenum pname, GLint param))
GL_FUNC(void,   glBindTextureUnit,       (GLuint unit, GLuint texture))
GL_FUNC(void,   glDeleteTextures,        (GLsizei n, const GLuint *textures))

// Framebuffers (DSA)
GL_FUNC(void,   glCreateFramebuffers,    (GLsizei n, GLuint *framebuffers))
GL_FUNC(void,   glNamedFramebufferTexture,(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level))
GL_FUNC(void,   glNamedFramebufferRenderbuffer,(GLuint framebuffer, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer))
GL_FUNC(GLenum, glCheckNamedFramebufferStatus, (GLuint framebuffer, GLenum target))
GL_FUNC(void,   glDeleteFramebuffers,    (GLsizei n, const GLuint *framebuffers))
GL_FUNC(void,   glBindFramebuffer,       (GLenum target, GLuint framebuffer))

// Drawing
GL_FUNC(void,   glDrawArrays,            (GLenum mode, GLint first, GLsizei count))
GL_FUNC(void,   glDrawElements,          (GLenum mode, GLsizei count, GLenum type, const void *indices))
GL_FUNC(void,   glDrawArraysInstanced,   (GLenum mode, GLint first, GLsizei count, GLsizei instancecount))
GL_FUNC(void,   glDrawElementsInstanced, (GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount))
GL_FUNC(void,   glClearColor,            (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha))

// Debug
GL_FUNC(void,            glDebugMessageCallback, (GLDEBUGPROC callback, const void *userParam))
GL_FUNC(void,            glEnable,               (GLenum cap)) // e.g., GL_DEBUG_OUTPUT
GL_FUNC(const GLubyte *, glGetString,            (GLenum name))
