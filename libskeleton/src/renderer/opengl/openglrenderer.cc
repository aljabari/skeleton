// Copyright 2026 aljabari

#include "libskeleton/opengl/openglrenderer.h"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <string>
#include <vector>

#include "libskeleton/scene.h"
#include "renderer/opengl/gl.h"
#include "renderer/opengl/openglframebuffer.h"
#include "renderer/opengl/openglmesh.h"
#include "renderer/opengl/openglshader.h"

namespace skeleton {

OpenGlRenderer::OpenGlRenderer(RenderTarget render_target)
    : Renderer(render_target) {}

OpenGlRenderer::~OpenGlRenderer() {
  if (window_ != nullptr) {
    // Keep the context current so the GL objects below are deleted while their
    // context still exists.
    glfwMakeContextCurrent(window_);
  }
  framebuffer_.reset();
  scene_meshes_.clear();
  shader_.reset();
  if (window_ != nullptr) {
    glfwDestroyWindow(window_);
  }
}

RendererBackend OpenGlRenderer::GetBackend() const {
  return RendererBackend::kOpenGl;
}

void OpenGlRenderer::CreateContext(const WindowConfig& config) {
  InitGlfw();
#if defined(__EMSCRIPTEN__)
  // Emscripten targets WebGL 2, which exposes OpenGL ES 3.0.
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#else
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#endif
  window_ = CreateGlfwWindow(config);

  glfwMakeContextCurrent(window_);
#if !defined(__EMSCRIPTEN__)
  gladLoadGL(glfwGetProcAddress);
#endif
  SPDLOG_INFO("Initialised OpenGL renderer.");

  const std::string shader_directory = SKELETON_SHADER_DIR;
  shader_ = std::make_unique<OpenGlShader>(
      shader_directory + "/triangle.vert.spv",
      shader_directory + "/triangle.frag.spv");

  if (render_target_ == RenderTarget::kRenderTargetTexture) {
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window_, &width, &height);
    CreateRenderTarget(width, height);
  }
  SPDLOG_INFO("Created OpenGL context for window \"{}\" ({}x{}).",
              config.title, config.width, config.height);
}

GLFWwindow* OpenGlRenderer::GetNativeWindow() const {
  return window_;
}

void OpenGlRenderer::CreateRenderTarget(int width, int height) {
  framebuffer_ = std::make_unique<OpenGlFramebuffer>(width, height);
  render_target_width_ = width;
  render_target_height_ = height;
}

void OpenGlRenderer::ResizeRenderTarget(int width, int height) {
  if (render_target_ != RenderTarget::kRenderTargetTexture || width <= 0 ||
      height <= 0) {
    return;
  }
  if (width == render_target_width_ && height == render_target_height_) {
    return;
  }
  framebuffer_->Resize(width, height);
  render_target_width_ = width;
  render_target_height_ = height;
}

unsigned int OpenGlRenderer::GetTextureId() const {
  return framebuffer_ != nullptr ? framebuffer_->GetTextureId() : 0;
}

void OpenGlRenderer::Render(const Scene& scene) {
  int viewport_width = 0;
  int viewport_height = 0;
  if (render_target_ == RenderTarget::kRenderTargetTexture) {
    framebuffer_->Bind();
    viewport_width = render_target_width_;
    viewport_height = render_target_height_;
  } else {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glfwGetFramebufferSize(window_, &viewport_width, &viewport_height);
  }
  glViewport(0, 0, viewport_width, viewport_height);

  glClearColor(0.2f, 0.3f, 0.8f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  if (shader_ == nullptr) {
    SPDLOG_WARN("OpenGL renderer has no shader; skipping draw.");
  } else {
    shader_->Use();

    const auto mesh_view = scene.Registry().view<MeshComponent>();
    scene_meshes_.resize(mesh_view.size());
    scene_mesh_vertices_.resize(mesh_view.size());
    std::size_t index = 0;
    for (entt::entity entity : mesh_view) {
      const MeshComponent& mesh_component =
          mesh_view.get<MeshComponent>(entity);
      if (scene_meshes_[index] == nullptr ||
          scene_mesh_vertices_[index] != mesh_component.vertices) {
        scene_mesh_vertices_[index] = mesh_component.vertices;
        scene_meshes_[index] =
            std::make_unique<OpenGlMesh>(mesh_component.vertices);
      }
      scene_meshes_[index]->Draw();
      ++index;
    }
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}  // namespace skeleton
