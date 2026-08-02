// Copyright 2026 aljabari

#ifndef LIBSKELETON_OPENGL_OPENGLRENDERER_H_
#define LIBSKELETON_OPENGL_OPENGLRENDERER_H_

#include "libskeleton/renderer.h"

struct GLFWwindow;

namespace skeleton {

class OpenGlRenderer : public Renderer {
 public:
  OpenGlRenderer();
  ~OpenGlRenderer() override;

  void SetWindowHints() override;
  void InitialiseForWindow(GLFWwindow* window) override;
  void Render() override;
};

}  // namespace skeleton

#endif  // LIBSKELETON_OPENGL_OPENGLRENDERER_H_
