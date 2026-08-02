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
  explicit OpenGlRenderer(bool render_to_texture = false);
  ~OpenGlRenderer() override;

  void SetWindowHints() override;
  void InitialiseForWindow(GLFWwindow* window) override;
  void Render() override;

  unsigned int GetTextureId() const;

 private:
  void CreateRenderTarget(int width, int height);

  std::unique_ptr<OpenGlShader> shader_;
  std::unique_ptr<OpenGlMesh> mesh_;
  unsigned int framebuffer_ = 0;
  unsigned int texture_ = 0;
  int render_target_width_ = 0;
  int render_target_height_ = 0;
};

}  // namespace skeleton

#endif  // LIBSKELETON_OPENGL_OPENGLRENDERER_H_
