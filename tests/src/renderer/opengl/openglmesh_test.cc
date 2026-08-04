// Copyright 2026 aljabari

#include "renderer/opengl/openglmesh.h"

#include <glad/gl.h>

#include <GLFW/glfw3.h>
#include <gtest/gtest.h>

#include <vector>

namespace skeleton {
namespace {

const std::vector<float> kTriangleVertices = {
    -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  //
    0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f,  //
    0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  //
};

class OpenGlMeshTest : public ::testing::Test {
 protected:
  void SetUp() override {
    ASSERT_TRUE(glfwInit());
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    window_ = glfwCreateWindow(640, 480, "Mesh Test", nullptr, nullptr);
    ASSERT_NE(window_, nullptr);
    glfwMakeContextCurrent(window_);
    gladLoadGL(glfwGetProcAddress);
  }

  void TearDown() override {
    glfwDestroyWindow(window_);
    glfwTerminate();
  }

  GLFWwindow* window_ = nullptr;
};

TEST_F(OpenGlMeshTest, BindBindsVertexArray) {
  const OpenGlMesh mesh(kTriangleVertices);

  mesh.Bind();

  GLint bound_vao = 0;
  glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &bound_vao);
  EXPECT_NE(bound_vao, 0);
  EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

TEST_F(OpenGlMeshTest, DrawRaisesNoErrors) {
  const OpenGlMesh mesh(kTriangleVertices);

  mesh.Draw();

  EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

}  // namespace
}  // namespace skeleton
