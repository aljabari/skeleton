// Copyright 2026 aljabari

#include "libskeleton/window.h"

#include <GLFW/glfw3.h>

namespace skeleton {

Window::Window(Renderer& renderer) : renderer_(renderer) {}

Window::~Window() = default;

bool Window::IsOpen() const {
  GLFWwindow* window = renderer_.GetNativeWindow();
  return window != nullptr && glfwWindowShouldClose(window) == 0;
}

GLFWwindow* Window::GetNativeWindow() {
  return renderer_.GetNativeWindow();
}

void Window::Maximize() {
  GLFWwindow* window = renderer_.GetNativeWindow();
  if (window != nullptr) {
    glfwMaximizeWindow(window);
  }
}

void Window::SwapBuffers() {
  GLFWwindow* window = renderer_.GetNativeWindow();
  if (window != nullptr) {
    glfwSwapBuffers(window);
  }
}

void Window::PollEvents() {
  glfwPollEvents();
}

}  // namespace skeleton
