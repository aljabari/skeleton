// Copyright 2026 aljabari

#include "libskeleton/opengl/openglrenderer.h"

#include <GLFW/glfw3.h>

namespace skeleton {

OpenGlRenderer::OpenGlRenderer() = default;

OpenGlRenderer::~OpenGlRenderer() = default;

void OpenGlRenderer::SetWindowHints() {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
}

void OpenGlRenderer::Render() {}

}  // namespace skeleton
