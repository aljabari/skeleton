// Copyright 2026 aljabari

#include "renderer/opengl/opengltexture.h"

#include <glad/gl.h>

namespace skeleton {

OpenGlTexture::OpenGlTexture(int width, int height) : texture_(0) {
  glGenTextures(1, &texture_);
  glBindTexture(GL_TEXTURE_2D, texture_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glBindTexture(GL_TEXTURE_2D, 0);
}

OpenGlTexture::~OpenGlTexture() {
  if (texture_ != 0) {
    glDeleteTextures(1, &texture_);
  }
}

GLuint OpenGlTexture::GetId() const {
  return texture_;
}

void OpenGlTexture::Bind() const {
  glBindTexture(GL_TEXTURE_2D, texture_);
}

}  // namespace skeleton
