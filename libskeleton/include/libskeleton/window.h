// Copyright 2026 aljabari

#ifndef LIBSKELETON_WINDOW_H_
#define LIBSKELETON_WINDOW_H_

#include "libskeleton/renderer.h"

struct GLFWwindow;

namespace skeleton {

class Window {
 public:
  Window(int width, int height, const char* title, Renderer& renderer);
  ~Window();

  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;

  bool IsOpen() const;
  void SwapBuffers();
  static void PollEvents();

 private:
  Renderer& renderer_;
  GLFWwindow* window_;
};

}  // namespace skeleton

#endif  // LIBSKELETON_WINDOW_H_
