// Copyright 2026 aljabari

#include "libskeleton/opengl/openglrenderer.h"

#include <glad/gl.h>

#include <GLFW/glfw3.h>
#include <gtest/gtest.h>

#include "libskeleton/window.h"

namespace skeleton {
namespace {

TEST(OpenGlRendererTest, HintsCreateOpenGl33Context) {
  ASSERT_TRUE(glfwInit());

  OpenGlRenderer renderer;
  renderer.SetWindowHints();

  GLFWwindow* window =
      glfwCreateWindow(640, 480, "OpenGL Hint Test", nullptr, nullptr);
  ASSERT_NE(window, nullptr);
  EXPECT_EQ(glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR), 3);
  EXPECT_EQ(glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR), 3);

  glfwDestroyWindow(window);
  glfwTerminate();
}

TEST(OpenGlRendererTest, InitialiseForWindowMakesContextCurrent) {
  ASSERT_TRUE(glfwInit());

  OpenGlRenderer renderer;
  renderer.SetWindowHints();

  GLFWwindow* window =
      glfwCreateWindow(640, 480, "OpenGL Context Test", nullptr, nullptr);
  ASSERT_NE(window, nullptr);

  renderer.InitialiseForWindow(window);
  EXPECT_EQ(glfwGetCurrentContext(), window);
  EXPECT_NE(glGetString(GL_VERSION), nullptr);

  glfwDestroyWindow(window);
  glfwTerminate();
}

TEST(OpenGlContextTest, IsCurrentAfterWindowConstruction) {
  OpenGlRenderer renderer;
  Window window(640, 480, "Skeleton Context Test", renderer);

  EXPECT_NE(glfwGetCurrentContext(), nullptr);
}

TEST(OpenGlContextTest, GladLoadsOpenGlFunctions) {
  OpenGlRenderer renderer;
  Window window(640, 480, "Skeleton Glad Test", renderer);

  const GLubyte* version = glGetString(GL_VERSION);
  ASSERT_NE(version, nullptr);

  int major = 0;
  int minor = 0;
  glGetIntegerv(GL_MAJOR_VERSION, &major);
  glGetIntegerv(GL_MINOR_VERSION, &minor);
  EXPECT_GE(major, 3);
  if (major == 3) {
    EXPECT_GE(minor, 3);
  }
}

}  // namespace
}  // namespace skeleton
