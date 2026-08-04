// Copyright 2026 aljabari

#include "libskeleton/window.h"

#include <GLFW/glfw3.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "libskeleton/renderer.h"

namespace skeleton {
namespace {

class MockRenderer : public Renderer {
 public:
  MOCK_METHOD(RendererBackend, GetBackend, (), (const, override));
  MOCK_METHOD(void, CreateContext, (const WindowConfig&), (override));
  MOCK_METHOD(void, Render, (), (override));

  GLFWwindow* GetNativeWindow() const override { return native_window_; }
  void SetNativeWindow(GLFWwindow* window) { native_window_ = window; }

 private:
  GLFWwindow* native_window_ = nullptr;
};

// Creates a real GLFW window and attaches it to |renderer|. The caller must
// destroy the window and call glfwTerminate.
void CreateNativeWindow(MockRenderer* renderer, GLFWwindow** out_window) {
  ASSERT_TRUE(glfwInit());
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  *out_window =
      glfwCreateWindow(800, 600, "Skeleton Test Window", nullptr, nullptr);
  ASSERT_NE(*out_window, nullptr);
  renderer->SetNativeWindow(*out_window);
}

TEST(WindowTest, GetNativeWindowReturnsRendererWindow) {
  MockRenderer renderer;
  GLFWwindow* native = nullptr;
  CreateNativeWindow(&renderer, &native);

  Window window(renderer);

  EXPECT_EQ(window.GetNativeWindow(), native);

  glfwDestroyWindow(native);
  glfwTerminate();
}

TEST(WindowTest, IsOpenReturnsTrueWhenRendererHasWindow) {
  MockRenderer renderer;
  GLFWwindow* native = nullptr;
  CreateNativeWindow(&renderer, &native);

  Window window(renderer);

  EXPECT_TRUE(window.IsOpen());

  glfwDestroyWindow(native);
  glfwTerminate();
}

TEST(WindowTest, IsOpenReturnsFalseWhenCloseRequested) {
  MockRenderer renderer;
  GLFWwindow* native = nullptr;
  CreateNativeWindow(&renderer, &native);

  Window window(renderer);
  glfwSetWindowShouldClose(native, GLFW_TRUE);

  EXPECT_FALSE(window.IsOpen());

  glfwDestroyWindow(native);
  glfwTerminate();
}

TEST(WindowTest, IsOpenReturnsFalseWithoutAWindow) {
  MockRenderer renderer;

  Window window(renderer);

  EXPECT_FALSE(window.IsOpen());
}

TEST(WindowTest, MaximizeMaximisesWindow) {
  MockRenderer renderer;
  GLFWwindow* native = nullptr;
  CreateNativeWindow(&renderer, &native);

  Window window(renderer);
  window.Maximize();

  EXPECT_EQ(glfwGetWindowAttrib(native, GLFW_MAXIMIZED), GLFW_TRUE);

  glfwDestroyWindow(native);
  glfwTerminate();
}

TEST(WindowTest, MultipleWindowsCanCoexist) {
  MockRenderer renderer_one;
  GLFWwindow* first = nullptr;
  CreateNativeWindow(&renderer_one, &first);
  MockRenderer renderer_two;
  GLFWwindow* second = nullptr;
  CreateNativeWindow(&renderer_two, &second);

  Window window_one(renderer_one);
  Window window_two(renderer_two);

  EXPECT_TRUE(window_one.IsOpen());
  EXPECT_TRUE(window_two.IsOpen());

  glfwDestroyWindow(first);
  glfwDestroyWindow(second);
  glfwTerminate();
}

TEST(WindowTest, PollEventsDoesNotCrash) {
  MockRenderer renderer;
  GLFWwindow* native = nullptr;
  CreateNativeWindow(&renderer, &native);

  Window window(renderer);
  Window::PollEvents();

  glfwDestroyWindow(native);
  glfwTerminate();
}

}  // namespace
}  // namespace skeleton
