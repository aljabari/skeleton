// Copyright 2026 aljabari

#include "renderer/opengl/openglshader.h"

#include <glad/gl.h>

#include <GLFW/glfw3.h>
#include <gtest/gtest.h>

#include <string>

namespace skeleton {
namespace {

constexpr char kShaderDir[] = SKELETON_RES_DIR "/shaders";

class OpenGlShaderTest : public ::testing::Test {
 protected:
  void SetUp() override {
    ASSERT_TRUE(glfwInit());
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    window_ = glfwCreateWindow(640, 480, "Shader Test", nullptr, nullptr);
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

TEST_F(OpenGlShaderTest, CompilesAndLinksShaders) {
  const OpenGlShader shader(std::string(kShaderDir) + "/triangle.vert",
                            std::string(kShaderDir) + "/triangle.frag");

  EXPECT_NE(shader.GetId(), 0);
}

TEST_F(OpenGlShaderTest, UseBindsShaderProgram) {
  const OpenGlShader shader(std::string(kShaderDir) + "/triangle.vert",
                            std::string(kShaderDir) + "/triangle.frag");

  shader.Use();

  GLint current_program = 0;
  glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);
  EXPECT_EQ(current_program, static_cast<GLint>(shader.GetId()));
  EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

}  // namespace
}  // namespace skeleton
