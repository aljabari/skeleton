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

// The same hardcoded triangle mesh the Vulkan renderer draws, authored in the
// Vulkan coordinate system (front faces wind counter-clockwise in the
// y-down framebuffer): three vertices of interleaved position (vec3) and
// colour (vec3).
const std::vector<float> kTriangleVertices = {
    -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  //
    0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f,  //
    0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  //
};

}  // namespace

OpenGlRenderer::OpenGlRenderer(RenderTarget render_target)
    : Renderer(render_target) {}

OpenGlRenderer::~OpenGlRenderer() {
  if (window_ != nullptr) {
    // Keep the context current so the GL objects below are deleted while their
    // context still exists.
    glfwMakeContextCurrent(window_);
  }
  framebuffer_.reset();
  mesh_.reset();
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
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  window_ = CreateGlfwWindow(config);

  glfwMakeContextCurrent(window_);
  gladLoadGL(glfwGetProcAddress);
  SPDLOG_INFO("Initialised OpenGL renderer.");

  const std::string shader_directory = SKELETON_SHADER_DIR;
  shader_ = std::make_unique<OpenGlShader>(
      shader_directory + "/triangle.vert.spv",
      shader_directory + "/triangle.frag.spv");
  mesh_ = std::make_unique<OpenGlMesh>(kTriangleVertices);

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
    SPDLOG_WARN("OpenGL renderer has no shader or mesh; skipping draw.");
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}  // namespace skeleton
