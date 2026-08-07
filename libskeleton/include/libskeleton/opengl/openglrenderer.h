// Copyright 2026 aljabari

#ifndef LIBSKELETON_OPENGL_OPENGLRENDERER_H_
#define LIBSKELETON_OPENGL_OPENGLRENDERER_H_

#include <memory>
#include <vector>

#include "libskeleton/renderer.h"

struct GLFWwindow;

namespace skeleton {

class OpenGlFramebuffer;
class OpenGlMesh;
class OpenGlShader;

class OpenGlRenderer : public Renderer {
 public:
  explicit OpenGlRenderer(
      RenderTarget render_target = RenderTarget::kRenderTargetWindow);
  ~OpenGlRenderer() override;

  RendererBackend GetBackend() const override;
  void CreateContext(const WindowConfig& config) override;
  GLFWwindow* GetNativeWindow() const override;
  void Render(const Scene& scene) override;
  void ResizeRenderTarget(int width, int height) override;

  unsigned int GetTextureId() const override;

 private:
  void CreateRenderTarget(int width, int height);

  GLFWwindow* window_ = nullptr;
  std::unique_ptr<OpenGlShader> shader_;
  // One GPU mesh per scene MeshComponent, rebuilt when a component's vertices
  // change. Kept alongside the source vertices so Render can detect changes.
  std::vector<std::unique_ptr<OpenGlMesh>> scene_meshes_;
  std::vector<std::vector<float>> scene_mesh_vertices_;
  std::unique_ptr<OpenGlFramebuffer> framebuffer_;
  int render_target_width_ = 0;
  int render_target_height_ = 0;
};

}  // namespace skeleton

#endif  // LIBSKELETON_OPENGL_OPENGLRENDERER_H_
