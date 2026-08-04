// Copyright 2026 aljabari

#include "libskeleton/renderer.h"

#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

namespace skeleton {

Renderer::Renderer(RenderTarget render_target)
    : render_target_(render_target) {}

Renderer::~Renderer() = default;

void Renderer::InitGlfw() {
  if (glfwInit() != GLFW_TRUE) {
    SPDLOG_ERROR("Failed to initialise GLFW.");
    throw RendererCreationException("Failed to initialise GLFW.");
  }
}

GLFWwindow* Renderer::CreateGlfwWindow(const WindowConfig& config) {
  GLFWwindow* window = glfwCreateWindow(config.width, config.height,
                                        config.title, nullptr, nullptr);
  if (window == nullptr) {
    SPDLOG_ERROR("Failed to create GLFW window.");
    throw RendererCreationException("Failed to create GLFW window.");
  }
  return window;
}

unsigned int Renderer::GetTextureId() const {
  return 0;
}

void Renderer::ResizeRenderTarget(int width, int height) {}

}  // namespace skeleton
