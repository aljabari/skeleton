// Copyright 2026 aljabari

#include "renderer/vulkan/vulkancommandbuffer.h"

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

TEST(VulkanCommandBufferTest, CreatesCommandPoolAndBuffer) {
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
  {
    VulkanDevice device(instance, surface);
    const VulkanCommandBuffer command_buffer(device);

    EXPECT_NE(command_buffer.CommandPool(), VK_NULL_HANDLE);
    EXPECT_NE(command_buffer.CommandBuffer(), VK_NULL_HANDLE);
  }

  vkDestroySurfaceKHR(instance.Instance(), surface, nullptr);
  glfwDestroyWindow(window);
  glfwTerminate();
}

}  // namespace
}  // namespace skeleton
