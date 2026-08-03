// Copyright 2026 aljabari

#include "renderer/opengl/opengltexture.h"

#include <glad/gl.h>

#include <GLFW/glfw3.h>
#include <gtest/gtest.h>

namespace skeleton {
namespace {

class OpenGlTextureTest : public ::testing::Test {
 protected:
  void SetUp() override {
    ASSERT_TRUE(glfwInit());
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    window_ = glfwCreateWindow(640, 480, "Texture Test", nullptr, nullptr);
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

TEST_F(OpenGlTextureTest, CreatesTexture) {
  const OpenGlTexture texture(640, 480);

  EXPECT_NE(texture.GetId(), 0);
  EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

TEST_F(OpenGlTextureTest, BindBindsTexture) {
  const OpenGlTexture texture(640, 480);

  texture.Bind();

  GLint bound_texture = 0;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound_texture);
  EXPECT_EQ(bound_texture, static_cast<GLint>(texture.GetId()));
  EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

TEST_F(OpenGlTextureTest, DestructorDeletesTexture) {
  GLuint texture_id = 0;
  {
    const OpenGlTexture texture(640, 480);
    texture_id = texture.GetId();
    EXPECT_TRUE(glIsTexture(texture_id));
  }

  EXPECT_FALSE(glIsTexture(texture_id));
}

}  // namespace
}  // namespace skeleton
