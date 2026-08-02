// Copyright 2026 aljabari

#ifndef LIBSKELETON_OPENGL_OPENGLRENDERER_H_
#define LIBSKELETON_OPENGL_OPENGLRENDERER_H_

#include "libskeleton/renderer.h"

namespace skeleton {

class OpenGlRenderer : public Renderer {
 public:
  OpenGlRenderer();
  ~OpenGlRenderer() override;

  void SetWindowHints() override;
  void Render() override;
};

}  // namespace skeleton

#endif  // LIBSKELETON_OPENGL_OPENGLRENDERER_H_
