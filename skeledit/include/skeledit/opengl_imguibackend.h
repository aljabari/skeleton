// Copyright 2026 aljabari

#ifndef SKELEDIT_OPENGL_IMGUIBACKEND_H_
#define SKELEDIT_OPENGL_IMGUIBACKEND_H_

#include "skeledit/imguibackend.h"

struct GLFWwindow;

namespace skeleton {

class Renderer;

// Pairs the GLFW platform backend with the ImGui OpenGL3 renderer backend.
// The OpenGL context must be current for the lifetime of the backend. The
// viewport texture identifier comes from the renderer's render target texture.
class OpenGlImGuiBackend : public ImGuiBackend {
 public:
  OpenGlImGuiBackend(GLFWwindow* window, Renderer* renderer);
  ~OpenGlImGuiBackend() override;

  OpenGlImGuiBackend(const OpenGlImGuiBackend&) = delete;
  OpenGlImGuiBackend& operator=(const OpenGlImGuiBackend&) = delete;

  bool Init() override;
  void NewFrame() override;
  void RenderDrawData() override;
  ImTextureID GetViewportTextureId() const override;
  bool FlipsViewportTexture() const override;
  void Shutdown() override;

 private:
  GLFWwindow* window_;
  Renderer* renderer_;
  bool initialised_ = false;
};

}  // namespace skeleton

#endif  // SKELEDIT_OPENGL_IMGUIBACKEND_H_
