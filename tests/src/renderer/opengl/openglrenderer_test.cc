// Copyright 2026 aljabari

#include "libskeleton/opengl/openglrenderer.h"

#include <glad/gl.h>

#include <GLFW/glfw3.h>
#include <gtest/gtest.h>

#include <vector>

#include "libskeleton/renderer.h"
#include "libskeleton/scene.h"

namespace skeleton {
namespace {

// The same hardcoded triangle mesh the Vulkan renderer tests use, authored in
// the Vulkan coordinate system: three vertices of interleaved position (vec3)
// and colour (vec3).
const std::vector<float> kTriangleVertices = {
    -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  //
    0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f,  //
    0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  //
};

// A scene containing a single triangle mesh entity.
Scene CreateTriangleScene() {
  Scene scene;
  const entt::entity triangle = scene.Registry().create();
  scene.Registry().emplace<MeshComponent>(triangle, kTriangleVertices);
  return scene;
}

TEST(OpenGlRendererTest, CreateContextCreatesOpenGl33Window) {
  OpenGlRenderer renderer;
  renderer.CreateContext(WindowConfig{640, 480, "OpenGL Hint Test"});

  GLFWwindow* window = renderer.GetNativeWindow();
  ASSERT_NE(window, nullptr);
  EXPECT_EQ(glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR), 3);
  EXPECT_EQ(glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR), 3);
}

TEST(OpenGlRendererTest, CreateContextMakesContextCurrent) {
  OpenGlRenderer renderer;
  renderer.CreateContext(WindowConfig{640, 480, "OpenGL Context Test"});

  EXPECT_EQ(glfwGetCurrentContext(), renderer.GetNativeWindow());
  EXPECT_NE(glGetString(GL_VERSION), nullptr);
}

TEST(OpenGlContextTest, ContextIsCurrentAfterCreateContext) {
  OpenGlRenderer renderer;
  renderer.CreateContext(WindowConfig{640, 480, "Skeleton Context Test"});

  EXPECT_NE(glfwGetCurrentContext(), nullptr);
}

TEST(OpenGlContextTest, GladLoadsOpenGlFunctions) {
  OpenGlRenderer renderer;
  renderer.CreateContext(WindowConfig{640, 480, "Skeleton Glad Test"});

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
  renderer.CreateContext(
      WindowConfig{640, 480, "Skeleton Render Triangle Test"});

  const Scene scene = CreateTriangleScene();

  renderer.Render(scene);

  EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

TEST(OpenGlRendererTest, ResizeRenderTargetRecreatesRenderTarget) {
  OpenGlRenderer renderer(RenderTarget::kRenderTargetTexture);
  renderer.CreateContext(
      WindowConfig{640, 480, "Skeleton Render Target Resize Test"});

  const unsigned int initial_texture_id = renderer.GetTextureId();
  EXPECT_NE(initial_texture_id, 0u);

  renderer.ResizeRenderTarget(320, 200);

  EXPECT_NE(renderer.GetTextureId(), 0u);
  EXPECT_NE(renderer.GetTextureId(), initial_texture_id);

  const Scene scene = CreateTriangleScene();
  renderer.Render(scene);

  EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

TEST(OpenGlRendererTest, ResizeRenderTargetToSameSizeKeepsTexture) {
  OpenGlRenderer renderer(RenderTarget::kRenderTargetTexture);
  renderer.CreateContext(
      WindowConfig{640, 480, "Skeleton Render Target No-Op Test"});

  renderer.ResizeRenderTarget(320, 200);
  const unsigned int resized_texture_id = renderer.GetTextureId();

  renderer.ResizeRenderTarget(320, 200);

  EXPECT_EQ(renderer.GetTextureId(), resized_texture_id);
}

TEST(OpenGlRendererTest, RenderSetsViewportToWindowFramebufferSize) {
  OpenGlRenderer renderer;
  renderer.CreateContext(
      WindowConfig{640, 480, "Skeleton Window Viewport Test"});

  int expected_width = 0;
  int expected_height = 0;
  glfwGetFramebufferSize(renderer.GetNativeWindow(), &expected_width,
                         &expected_height);

  const Scene scene = CreateTriangleScene();
  renderer.Render(scene);

  GLint viewport[4] = {0, 0, 0, 0};
  glGetIntegerv(GL_VIEWPORT, viewport);
  EXPECT_EQ(viewport[0], 0);
  EXPECT_EQ(viewport[1], 0);
  EXPECT_EQ(viewport[2], expected_width);
  EXPECT_EQ(viewport[3], expected_height);
  EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

TEST(OpenGlRendererTest, RenderSetsViewportToRenderTargetSize) {
  OpenGlRenderer renderer(RenderTarget::kRenderTargetTexture);
  renderer.CreateContext(
      WindowConfig{640, 480, "Skeleton Render Target Viewport Test"});

  renderer.ResizeRenderTarget(320, 200);
  const Scene scene = CreateTriangleScene();
  renderer.Render(scene);

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
