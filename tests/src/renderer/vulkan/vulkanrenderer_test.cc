// Copyright 2026 aljabari

#include "libskeleton/vulkan/vulkanrenderer.h"

#include <GLFW/glfw3.h>
#include <gtest/gtest.h>

#include "libskeleton/window.h"

namespace skeleton {
namespace {

TEST(VulkanRendererTest, HintsDisableClientApi) {
  ASSERT_TRUE(glfwInit());

  VulkanRenderer renderer;
  renderer.SetWindowHints();

  GLFWwindow* window =
      glfwCreateWindow(640, 480, "Vulkan Hint Test", nullptr, nullptr);
  ASSERT_NE(window, nullptr);
  EXPECT_EQ(glfwGetWindowAttrib(window, GLFW_CLIENT_API), GLFW_NO_API);

  glfwDestroyWindow(window);
  glfwTerminate();
}

}  // namespace
}  // namespace skeleton
