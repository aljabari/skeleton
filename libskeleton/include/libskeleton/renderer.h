// Copyright 2026 aljabari

#ifndef LIBSKELETON_RENDERER_H_
#define LIBSKELETON_RENDERER_H_

struct GLFWwindow;

namespace skeleton {

// Available renderer backends. Only backends compiled for the target platform
// can be created; the platform fallback order is defined in
// libskeleton/rendererfactory.h.
enum class RendererBackend {
  kVulkan,
  kOpenGl,
};

class Renderer {
 public:
  virtual ~Renderer();

  virtual RendererBackend GetBackend() const = 0;
  virtual void SetWindowHints() = 0;
  virtual void InitialiseForWindow(GLFWwindow* window) = 0;
  virtual void Render() = 0;

  // Returns the identifier of the texture the renderer draws into when
  // rendering to a texture, or 0 when it does not.
  virtual unsigned int GetTextureId() const;

  // Recreates the render target at the given size. No-op for renderers that do
  // not render to a texture.
  virtual void ResizeRenderTarget(int width, int height);

 protected:
  explicit Renderer(bool render_to_texture = false);

  const bool render_to_texture_;
};

}  // namespace skeleton

#endif  // LIBSKELETON_RENDERER_H_
