// Copyright 2026 aljabari

#ifndef LIBSKELETON_SRC_RENDERER_OPENGL_OPENGLTEXTURE_H_
#define LIBSKELETON_SRC_RENDERER_OPENGL_OPENGLTEXTURE_H_

#include <glad/gl.h>

namespace skeleton {

// RAII wrapper around an OpenGL 2D texture. The texture is allocated from a
// width and height, and deleted on destruction.
class OpenGlTexture {
 public:
  OpenGlTexture(int width, int height);
  ~OpenGlTexture();

  OpenGlTexture(const OpenGlTexture&) = delete;
  OpenGlTexture& operator=(const OpenGlTexture&) = delete;

  GLuint GetId() const;
  void Bind() const;

 private:
  GLuint texture_;
};

}  // namespace skeleton

#endif  // LIBSKELETON_SRC_RENDERER_OPENGL_OPENGLTEXTURE_H_
