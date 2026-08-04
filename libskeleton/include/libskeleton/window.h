// Copyright 2026 aljabari

#ifndef LIBSKELETON_WINDOW_H_
#define LIBSKELETON_WINDOW_H_

#include "libskeleton/renderer.h"

struct GLFWwindow;

namespace skeleton {

// Wraps the window owned by a renderer (created during its context
// initialisation) with the GLFW operations used by the application loop. The
// window itself is created and destroyed by the renderer.
class Window {
 public:
  explicit Window(Renderer& renderer);
  ~Window();

  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;

  bool IsOpen() const;
  GLFWwindow* GetNativeWindow();
  void Maximize();
  void SwapBuffers();
  static void PollEvents();

 private:
  Renderer& renderer_;
};

}  // namespace skeleton

#endif  // LIBSKELETON_WINDOW_H_
