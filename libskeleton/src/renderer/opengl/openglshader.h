// Copyright 2026 aljabari

#ifndef LIBSKELETON_SRC_RENDERER_OPENGL_OPENGLSHADER_H_
#define LIBSKELETON_SRC_RENDERER_OPENGL_OPENGLSHADER_H_

#include <glad/gl.h>

#include <string>

namespace skeleton {

// RAII wrapper around an OpenGL shader program. The program is compiled and
// linked from a vertex and a fragment shader file, and deleted on destruction.
class OpenGlShader {
 public:
  OpenGlShader(const std::string& vertex_path,
               const std::string& fragment_path);
  ~OpenGlShader();

  OpenGlShader(const OpenGlShader&) = delete;
  OpenGlShader& operator=(const OpenGlShader&) = delete;

  GLuint GetId() const;
  void Use() const;

 private:
  GLuint program_;
};

}  // namespace skeleton

#endif  // LIBSKELETON_SRC_RENDERER_OPENGL_OPENGLSHADER_H_
