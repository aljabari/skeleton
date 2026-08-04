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

// Describes the window a renderer creates as part of its context. Renderers
// own the window they create, so backends can be fully initialised inside the
// factory's fallback handling.
struct WindowConfig {
  int width = 1280;
  int height = 720;
  const char* title = "Skeleton";
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
  // Creates the window (with backend-specific hints) and initialises the
  // backend context, then owns the window for the renderer's lifetime. Throws
  // RendererCreationException on failure so the factory can fall back to the
  // next backend.
  virtual void CreateContext(const WindowConfig& config) = 0;
  // The window owned by this renderer, or nullptr before CreateContext.
  virtual GLFWwindow* GetNativeWindow() const = 0;
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

  // Initialises GLFW. Must run before the backend sets its window hints,
  // because glfwInit resets all window hints. Throws
  // RendererCreationException when GLFW cannot be initialised.
  void InitGlfw();

  // Creates a GLFW window from |config| using the hints the backend set after
  // InitGlfw. Throws RendererCreationException when the window cannot be
  // created.
  GLFWwindow* CreateGlfwWindow(const WindowConfig& config);

  const RenderTarget render_target_;
};

}  // namespace skeleton

#endif  // LIBSKELETON_RENDERER_H_
