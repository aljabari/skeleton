// Copyright 2026 aljabari

#include "libskeleton/vulkan/vulkanrenderer.h"

#include <GLFW/glfw3.h>
#include <gtest/gtest.h>

#include "libskeleton/renderer.h"

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

TEST(VulkanRendererTest, CreatesInstancePhysicalDeviceAndLogicalDevice) {
  SkipIfVulkanUnavailable();

  VulkanRenderer renderer;

  EXPECT_NE(renderer.Instance(), VK_NULL_HANDLE);
  EXPECT_NE(renderer.PhysicalDevice(), VK_NULL_HANDLE);
  EXPECT_NE(renderer.Device(), VK_NULL_HANDLE);
  EXPECT_NE(renderer.GraphicsQueue(), VK_NULL_HANDLE);

  glfwTerminate();
}

TEST(VulkanRendererTest, GetBackendReturnsVulkan) {
  SkipIfVulkanUnavailable();

  VulkanRenderer renderer;
  EXPECT_EQ(renderer.GetBackend(), RendererBackend::kVulkan);

  glfwTerminate();
}

}  // namespace
}  // namespace skeleton
