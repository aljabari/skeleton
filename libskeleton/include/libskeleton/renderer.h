// Copyright 2026 aljabari

#ifndef LIBSKELETON_RENDERER_H_
#define LIBSKELETON_RENDERER_H_

#include <exception>
#include <string>

struct GLFWwindow;

namespace skeleton {

// Available renderer backends. Only backends compiled for the target platform
// can be created; the platform fallback order is defined in
// libskeleton/rendererfactory.h.
enum class RendererBackend {
  kVulkan,
  kOpenGl,
};

// Where the renderer draws its output: directly into the window's default
// framebuffer, or into an off-screen texture.
enum class RenderTarget {
  kRenderTargetWindow,
  kRenderTargetTexture,
};

// Thrown when a renderer backend cannot be created. The renderer factory
// catches this exception to trigger fallback to the next backend.
class RendererCreationException : public std::exception {
 public:
  explicit RendererCreationException(const std::string& message)
      : message_(message) {}

  const char* what() const noexcept override { return message_.c_str(); }

 private:
  std::string message_;
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
  explicit Renderer(RenderTarget render_target =
                        RenderTarget::kRenderTargetWindow);

  const RenderTarget render_target_;
};

}  // namespace skeleton

#endif  // LIBSKELETON_RENDERER_H_
