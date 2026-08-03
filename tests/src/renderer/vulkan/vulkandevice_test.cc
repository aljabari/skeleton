// Copyright 2026 aljabari

#include "renderer/vulkan/vulkandevice.h"

#include <GLFW/glfw3.h>
#include <gtest/gtest.h>

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

TEST(VulkanDeviceTest, CreatesLogicalDevice) {
  SkipIfVulkanUnavailable();

  VulkanInstance instance;
  VulkanDevice device(instance);

  EXPECT_NE(device.PhysicalDevice(), VK_NULL_HANDLE);
  EXPECT_NE(device.Device(), VK_NULL_HANDLE);
  EXPECT_NE(device.GraphicsQueue(), VK_NULL_HANDLE);

  glfwTerminate();
}

}  // namespace
}  // namespace skeleton
