// Copyright 2026 aljabari

#include "libskeleton/opengl/openglrenderer.h"

#include <glad/gl.h>

#include <GLFW/glfw3.h>

#include <memory>
#include <string>
#include <vector>

#include "renderer/opengl/openglmesh.h"
#include "renderer/opengl/openglshader.h"
#include "renderer/opengl/opengltexture.h"

namespace skeleton {

namespace {

const std::vector<float> kTriangleVertices = {
    -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  //
    0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  //
    0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f,  //
};

}  // namespace

OpenGlRenderer::OpenGlRenderer(bool render_to_texture)
    : Renderer(render_to_texture) {}

OpenGlRenderer::~OpenGlRenderer() {
  if (framebuffer_ != 0) {
    glDeleteFramebuffers(1, &framebuffer_);
  }
}

void OpenGlRenderer::SetWindowHints() {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
}

void OpenGlRenderer::InitialiseForWindow(GLFWwindow* window) {
  glfwMakeContextCurrent(window);
  gladLoadGL(glfwGetProcAddress);

  const std::string shader_directory =
      std::string(SKELETON_RES_DIR) + "/shaders";
  shader_ = std::make_unique<OpenGlShader>(
      shader_directory + "/triangle.vert", shader_directory + "/triangle.frag");
  mesh_ = std::make_unique<OpenGlMesh>(kTriangleVertices);

  if (render_to_texture_) {
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    CreateRenderTarget(width, height);
  }
}

void OpenGlRenderer::CreateRenderTarget(int width, int height) {
  if (framebuffer_ != 0) {
    glDeleteFramebuffers(1, &framebuffer_);
    framebuffer_ = 0;
  }

  texture_ = std::make_unique<OpenGlTexture>(width, height);

  glGenFramebuffers(1, &framebuffer_);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         texture_->GetId(), 0);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  render_target_width_ = width;
  render_target_height_ = height;
}

void OpenGlRenderer::ResizeRenderTarget(int width, int height) {
  if (!render_to_texture_ || width <= 0 || height <= 0) {
    return;
  }
  if (width == render_target_width_ && height == render_target_height_) {
    return;
  }
  CreateRenderTarget(width, height);
}

unsigned int OpenGlRenderer::GetTextureId() const {
  return texture_ != nullptr ? texture_->GetId() : 0;
}

void OpenGlRenderer::Render() {
  glBindFramebuffer(GL_FRAMEBUFFER, render_to_texture_ ? framebuffer_ : 0);
  if (render_to_texture_) {
    glViewport(0, 0, render_target_width_, render_target_height_);
  }

  glClearColor(0.2f, 0.3f, 0.8f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  if (shader_ != nullptr && mesh_ != nullptr) {
    shader_->Use();
    mesh_->Draw();
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}  // namespace skeleton
