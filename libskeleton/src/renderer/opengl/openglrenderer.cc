// Copyright 2026 aljabari

#include "libskeleton/opengl/openglrenderer.h"

#include <glad/gl.h>

#include <GLFW/glfw3.h>

namespace skeleton {

OpenGlRenderer::OpenGlRenderer() = default;

OpenGlRenderer::~OpenGlRenderer() = default;

void OpenGlRenderer::SetWindowHints() {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
}

void OpenGlRenderer::InitialiseForWindow(GLFWwindow* window) {
  glfwMakeContextCurrent(window);
  gladLoadGL(glfwGetProcAddress);
}

void OpenGlRenderer::Render() {
  glClearColor(0.2f, 0.3f, 0.8f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}

}  // namespace skeleton
