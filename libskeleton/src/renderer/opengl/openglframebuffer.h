// Copyright 2026 aljabari

#ifndef LIBSKELETON_SRC_RENDERER_OPENGL_OPENGLFRAMEBUFFER_H_
#define LIBSKELETON_SRC_RENDERER_OPENGL_OPENGLFRAMEBUFFER_H_

#include <glad/gl.h>

#include <memory>

namespace skeleton {

class OpenGlTexture;

// RAII wrapper around an OpenGL framebuffer object with an attached colour
// texture. The framebuffer is allocated from a width and height, and deleted
// on destruction.
class OpenGlFramebuffer {
 public:
  OpenGlFramebuffer(int width, int height);
  ~OpenGlFramebuffer();

  OpenGlFramebuffer(const OpenGlFramebuffer&) = delete;
  OpenGlFramebuffer& operator=(const OpenGlFramebuffer&) = delete;

  GLuint GetId() const;
  GLuint GetTextureId() const;
  void Resize(int width, int height);
  void Bind() const;

 private:
  GLuint framebuffer_ = 0;
  std::unique_ptr<OpenGlTexture> texture_;
};

}  // namespace skeleton

#endif  // LIBSKELETON_SRC_RENDERER_OPENGL_OPENGLFRAMEBUFFER_H_
