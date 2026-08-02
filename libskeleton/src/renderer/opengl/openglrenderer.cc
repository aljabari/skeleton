// Copyright 2026 aljabari

#include "libskeleton/opengl/openglrenderer.h"

#include <glad/gl.h>

#include <GLFW/glfw3.h>

#include <memory>
#include <string>
#include <vector>

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

OpenGlRenderer::OpenGlRenderer() = default;

OpenGlRenderer::~OpenGlRenderer() = default;

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
}

void OpenGlRenderer::Render() {
  glClearColor(0.2f, 0.3f, 0.8f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  if (shader_ != nullptr && mesh_ != nullptr) {
    shader_->Use();
    mesh_->Draw();
  }
}

}  // namespace skeleton
