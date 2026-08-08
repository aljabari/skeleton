// Copyright 2026 aljabari

#include "renderer/opengl/openglframebuffer.h"

#include <memory>

#include "renderer/opengl/gl.h"
#include "renderer/opengl/opengltexture.h"

namespace skeleton {

OpenGlFramebuffer::OpenGlFramebuffer(int width, int height) {
  glGenFramebuffers(1, &framebuffer_);
  texture_ = std::make_unique<OpenGlTexture>(width, height);
  Bind();
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         texture_->GetId(), 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

OpenGlFramebuffer::~OpenGlFramebuffer() {
  if (framebuffer_ != 0) {
    glDeleteFramebuffers(1, &framebuffer_);
  }
}

GLuint OpenGlFramebuffer::GetId() const {
  return framebuffer_;
}

GLuint OpenGlFramebuffer::GetTextureId() const {
  return texture_ != nullptr ? texture_->GetId() : 0;
}

void OpenGlFramebuffer::Resize(int width, int height) {
  texture_ = std::make_unique<OpenGlTexture>(width, height);
  Bind();
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         texture_->GetId(), 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGlFramebuffer::Bind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
}

}  // namespace skeleton
