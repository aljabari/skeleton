// Copyright 2026 aljabari

#include "renderer/vulkan/vulkanswapchain.h"

#include <GLFW/glfw3.h>
#include <gtest/gtest.h>

#include <vector>

#include "renderer/vulkan/vulkandevice.h"
#include "renderer/vulkan/vulkaninstance.h"

namespace skeleton {
namespace {

// Initialises GLFW and skips the test when Vulkan is not available on the
// current system.
void SkipIfVulkanUnavailable() {
  ASSERT_TRUE(glfwInit());
  if (!glfwVulkanSupported()) {
    glfwTerminate();
    GTEST_SKIP() << "Vulkan not supported on this system";
  }
}

TEST(VulkanSwapchainTest, CreatesSwapchainAndImageViews) {
  SkipIfVulkanUnavailable();

  uint32_t extension_count = 0;
  const char** required_extensions =
      glfwGetRequiredInstanceExtensions(&extension_count);
  ASSERT_NE(required_extensions, nullptr);
  VulkanInstance instance(std::vector<const char*>(
      required_extensions, required_extensions + extension_count));
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  GLFWwindow* window = glfwCreateWindow(800, 600, "test", nullptr, nullptr);
  ASSERT_NE(window, nullptr);
  VkSurfaceKHR surface = VK_NULL_HANDLE;
  ASSERT_EQ(glfwCreateWindowSurface(instance.Instance(), window, nullptr,
                                    &surface),
            VK_SUCCESS);
  VulkanDevice device(instance, surface);
  {
    VulkanSwapchain swapchain(device, surface, 800, 600);

    EXPECT_NE(swapchain.Swapchain(), VK_NULL_HANDLE);
    EXPECT_NE(swapchain.Extent().width, 0u);
    EXPECT_NE(swapchain.Extent().height, 0u);
    EXPECT_NE(swapchain.ImageFormat(), VK_FORMAT_UNDEFINED);
    EXPECT_FALSE(swapchain.Images().empty());
    EXPECT_EQ(swapchain.ImageViews().size(), swapchain.Images().size());
  }

  glfwDestroyWindow(window);
  glfwTerminate();
}

}  // namespace
}  // namespace skeleton
