// Copyright 2026 aljabari

#include "renderer/vulkan/vulkaninstance.h"

#include <GLFW/glfw3.h>
#include <gtest/gtest.h>

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

TEST(VulkanInstanceTest, CreatesVulkanInstance) {
  SkipIfVulkanUnavailable();

  VulkanInstance instance;

  EXPECT_NE(instance.Instance(), VK_NULL_HANDLE);

  glfwTerminate();
}

TEST(VulkanInstanceTest, EnumeratesPhysicalDevices) {
  SkipIfVulkanUnavailable();

  VulkanInstance instance;
  const auto devices = instance.EnumeratePhysicalDevices();

  EXPECT_FALSE(devices.empty());
  for (VkPhysicalDevice device : devices) {
    EXPECT_NE(device, VK_NULL_HANDLE);
  }

  glfwTerminate();
}

#if SKELETON_VULKAN_ENABLE_VALIDATION
TEST(VulkanInstanceTest, CreatesDebugMessenger) {
  SkipIfVulkanUnavailable();

  VulkanInstance instance;

  EXPECT_NE(instance.DebugMessenger(), VK_NULL_HANDLE);

  glfwTerminate();
}
#endif  // SKELETON_VULKAN_ENABLE_VALIDATION

}  // namespace
}  // namespace skeleton
