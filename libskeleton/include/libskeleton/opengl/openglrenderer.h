// Copyright 2026 aljabari

#ifndef LIBSKELETON_OPENGL_OPENGLRENDERER_H_
#define LIBSKELETON_OPENGL_OPENGLRENDERER_H_

#include <memory>

#include "libskeleton/renderer.h"

struct GLFWwindow;

namespace skeleton {

class OpenGlMesh;
class OpenGlShader;

class OpenGlRenderer : public Renderer {
 public:
  OpenGlRenderer();
  ~OpenGlRenderer() override;

  void SetWindowHints() override;
  void InitialiseForWindow(GLFWwindow* window) override;
  void Render() override;

 private:
  std::unique_ptr<OpenGlShader> shader_;
  std::unique_ptr<OpenGlMesh> mesh_;
};

}  // namespace skeleton

#endif  // LIBSKELETON_OPENGL_OPENGLRENDERER_H_
