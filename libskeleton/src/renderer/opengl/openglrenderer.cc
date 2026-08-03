// Copyright 2026 aljabari

#include "libskeleton/opengl/openglrenderer.h"

#include <glad/gl.h>

#include <spdlog/spdlog.h>

#include <GLFW/glfw3.h>

#include <memory>
#include <string>
#include <vector>

#include "renderer/opengl/openglframebuffer.h"
#include "renderer/opengl/openglmesh.h"
#include "renderer/opengl/openglshader.h"

namespace skeleton {

namespace {

const std::vector<float> kTriangleVertices = {
    -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  //
    0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  //
    0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f,  //
};

}  // namespace

OpenGlRenderer::OpenGlRenderer(RenderTarget render_target)
    : Renderer(render_target) {}

OpenGlRenderer::~OpenGlRenderer() = default;

RendererBackend OpenGlRenderer::GetBackend() const {
  return RendererBackend::kOpenGl;
}

void OpenGlRenderer::SetWindowHints() {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
}

void OpenGlRenderer::InitialiseForWindow(GLFWwindow* window) {
  window_ = window;
  glfwMakeContextCurrent(window);
  gladLoadGL(glfwGetProcAddress);
  spdlog::info("Initialised OpenGL renderer.");

  const std::string shader_directory =
      std::string(SKELETON_RES_DIR) + "/shaders";
  shader_ = std::make_unique<OpenGlShader>(
      shader_directory + "/triangle.vert", shader_directory + "/triangle.frag");
  mesh_ = std::make_unique<OpenGlMesh>(kTriangleVertices);

  if (render_target_ == RenderTarget::kRenderTargetTexture) {
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    CreateRenderTarget(width, height);
  }
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

void OpenGlRenderer::Render() {
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

  if (shader_ != nullptr && mesh_ != nullptr) {
    shader_->Use();
    mesh_->Draw();
  } else {
    spdlog::warn("OpenGL renderer has no shader or mesh; skipping draw.");
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}  // namespace skeleton
