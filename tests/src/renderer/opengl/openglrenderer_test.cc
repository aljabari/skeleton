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

TEST(OpenGlRendererTest, RenderDrawsTriangleWithoutErrors) {
  OpenGlRenderer renderer;
  Window window(640, 480, "Skeleton Render Triangle Test", renderer);

  renderer.Render();

  EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

TEST(OpenGlRendererTest, ResizeRenderTargetRecreatesRenderTarget) {
  OpenGlRenderer renderer(true);
  Window window(640, 480, "Skeleton Render Target Resize Test", renderer);

  const unsigned int initial_texture_id = renderer.GetTextureId();
  EXPECT_NE(initial_texture_id, 0u);

  renderer.ResizeRenderTarget(320, 200);

  EXPECT_NE(renderer.GetTextureId(), 0u);
  EXPECT_NE(renderer.GetTextureId(), initial_texture_id);

  renderer.Render();

  EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

TEST(OpenGlRendererTest, ResizeRenderTargetToSameSizeKeepsTexture) {
  OpenGlRenderer renderer(true);
  Window window(640, 480, "Skeleton Render Target No-Op Test", renderer);

  renderer.ResizeRenderTarget(320, 200);
  const unsigned int resized_texture_id = renderer.GetTextureId();

  renderer.ResizeRenderTarget(320, 200);

  EXPECT_EQ(renderer.GetTextureId(), resized_texture_id);
}

TEST(OpenGlRendererTest, RenderSetsViewportToWindowFramebufferSize) {
  OpenGlRenderer renderer;
  Window window(640, 480, "Skeleton Window Viewport Test", renderer);

  int expected_width = 0;
  int expected_height = 0;
  glfwGetFramebufferSize(window.GetNativeWindow(), &expected_width,
                         &expected_height);

  renderer.Render();

  GLint viewport[4] = {0, 0, 0, 0};
  glGetIntegerv(GL_VIEWPORT, viewport);
  EXPECT_EQ(viewport[0], 0);
  EXPECT_EQ(viewport[1], 0);
  EXPECT_EQ(viewport[2], expected_width);
  EXPECT_EQ(viewport[3], expected_height);
  EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

TEST(OpenGlRendererTest, RenderSetsViewportToRenderTargetSize) {
  OpenGlRenderer renderer(true);
  Window window(640, 480, "Skeleton Render Target Viewport Test", renderer);

  renderer.ResizeRenderTarget(320, 200);
  renderer.Render();

  GLint viewport[4] = {0, 0, 0, 0};
  glGetIntegerv(GL_VIEWPORT, viewport);
  EXPECT_EQ(viewport[0], 0);
  EXPECT_EQ(viewport[1], 0);
  EXPECT_EQ(viewport[2], 320);
  EXPECT_EQ(viewport[3], 200);
  EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

}  // namespace
}  // namespace skeleton
