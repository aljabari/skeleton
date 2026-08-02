// Copyright 2026 aljabari

#ifndef LIBSKELETON_RENDERER_H_
#define LIBSKELETON_RENDERER_H_

struct GLFWwindow;

namespace skeleton {

class Renderer {
 public:
  virtual ~Renderer();

  virtual void SetWindowHints() = 0;
  virtual void InitialiseForWindow(GLFWwindow* window) = 0;
  virtual void Render() = 0;

 protected:
  explicit Renderer(bool render_to_texture = false);

  const bool render_to_texture_;
};

}  // namespace skeleton

#endif  // LIBSKELETON_RENDERER_H_
