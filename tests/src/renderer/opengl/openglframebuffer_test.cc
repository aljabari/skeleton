// Copyright 2026 aljabari

#include "renderer/opengl/openglframebuffer.h"

#include <glad/gl.h>

#include <GLFW/glfw3.h>
#include <gtest/gtest.h>

namespace skeleton {
namespace {

class OpenGlFramebufferTest : public ::testing::Test {
 protected:
  void SetUp() override {
    ASSERT_TRUE(glfwInit());
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    window_ = glfwCreateWindow(640, 480, "Framebuffer Test", nullptr, nullptr);
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

TEST_F(OpenGlFramebufferTest, CreatesFramebufferAndTexture) {
  const OpenGlFramebuffer framebuffer(640, 480);

  EXPECT_NE(framebuffer.GetId(), 0);
  EXPECT_NE(framebuffer.GetTextureId(), 0);
  EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

TEST_F(OpenGlFramebufferTest, BindBindsFramebuffer) {
  const OpenGlFramebuffer framebuffer(640, 480);

  framebuffer.Bind();

  GLint bound_framebuffer = 0;
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &bound_framebuffer);
  EXPECT_EQ(bound_framebuffer, static_cast<GLint>(framebuffer.GetId()));
  EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

TEST_F(OpenGlFramebufferTest, ResizeReplacesColourTexture) {
  OpenGlFramebuffer framebuffer(640, 480);
  const GLuint initial_texture = framebuffer.GetTextureId();

  framebuffer.Resize(320, 200);

  EXPECT_NE(framebuffer.GetTextureId(), initial_texture);
  EXPECT_NE(framebuffer.GetTextureId(), 0);
  EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

TEST_F(OpenGlFramebufferTest, DestructorDeletesFramebuffer) {
  GLuint framebuffer_id = 0;
  {
    const OpenGlFramebuffer framebuffer(640, 480);
    framebuffer_id = framebuffer.GetId();
    EXPECT_TRUE(glIsFramebuffer(framebuffer_id));
  }

  EXPECT_FALSE(glIsFramebuffer(framebuffer_id));
}

}  // namespace
}  // namespace skeleton
