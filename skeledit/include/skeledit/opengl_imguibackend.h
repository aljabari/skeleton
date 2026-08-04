// Copyright 2026 aljabari

#ifndef SKELEDIT_OPENGL_IMGUIBACKEND_H_
#define SKELEDIT_OPENGL_IMGUIBACKEND_H_

#include "skeledit/imguibackend.h"

struct GLFWwindow;

namespace skeleton {

// Pairs the GLFW platform backend with the ImGui OpenGL3 renderer backend.
// The OpenGL context must be current for the lifetime of the backend.
class OpenGlImGuiBackend : public ImGuiBackend {
 public:
  explicit OpenGlImGuiBackend(GLFWwindow* window);
  ~OpenGlImGuiBackend() override;

  OpenGlImGuiBackend(const OpenGlImGuiBackend&) = delete;
  OpenGlImGuiBackend& operator=(const OpenGlImGuiBackend&) = delete;

  bool Init() override;
  void NewFrame() override;
  void RenderDrawData() override;
  void Shutdown() override;

 private:
  GLFWwindow* window_;
  bool initialised_ = false;
};

}  // namespace skeleton

#endif  // SKELEDIT_OPENGL_IMGUIBACKEND_H_
