// Copyright 2026 aljabari

#include "libskeleton/window.h"

#include <GLFW/glfw3.h>

#include <iostream>

namespace skeleton {

Window::Window(int width, int height, const char* title, Renderer& renderer)
    : renderer_(renderer), window_(nullptr) {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW\n";
    return;
  }

  renderer_.SetWindowHints();

  window_ = glfwCreateWindow(width, height, title, nullptr, nullptr);
  if (!window_) {
    std::cerr << "Failed to create GLFW window\n";
    glfwTerminate();
    return;
  }

  renderer_.InitialiseForWindow(window_);
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

GLFWwindow* Window::GetNativeWindow() {
  return window_;
}

void Window::Maximize() {
  if (window_) {
    glfwMaximizeWindow(window_);
  }
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
