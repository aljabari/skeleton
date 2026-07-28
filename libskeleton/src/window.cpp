// Copyright 2026 aljabari

#include "libskeleton/window.h"

#include <GLFW/glfw3.h>

#include <iostream>

namespace skeleton {

Window::Window(int width, int height, const char* title)
    : window_(nullptr) {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW\n";
    return;
  }

  window_ = glfwCreateWindow(width, height, title, nullptr, nullptr);
  if (!window_) {
    std::cerr << "Failed to create GLFW window\n";
    glfwTerminate();
  }
}

Window::~Window() {
  if (window_) {
    glfwDestroyWindow(window_);
  }
  glfwTerminate();
}

bool Window::IsOpen() const {
  return window_ ? glfwWindowShouldClose(window_) == 0 : false;
}

void Window::SwapBuffers() {
  if (window_) {
    glfwSwapBuffers(window_);
  }
}

void Window::PollEvents() {
  glfwPollEvents();
}

}  // namespace skeleton
