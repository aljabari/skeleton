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

TEST(VulkanRendererTest, CreatesInstanceSurfaceAndLogicalDevice) {
  SkipIfVulkanUnavailable();

  {
    VulkanRenderer renderer;
    renderer.CreateContext(WindowConfig{800, 600, "test"});

    EXPECT_NE(renderer.Instance(), VK_NULL_HANDLE);
    EXPECT_NE(renderer.Surface(), VK_NULL_HANDLE);
    EXPECT_NE(renderer.PhysicalDevice(), VK_NULL_HANDLE);
    EXPECT_NE(renderer.Device(), VK_NULL_HANDLE);
    EXPECT_NE(renderer.GraphicsQueue(), VK_NULL_HANDLE);
    EXPECT_NE(renderer.PresentQueue(), VK_NULL_HANDLE);
    EXPECT_NE(renderer.GetNativeWindow(), nullptr);
  }

  glfwTerminate();
}

TEST(VulkanRendererTest, GetBackendReturnsVulkan) {
  VulkanRenderer renderer;

  EXPECT_EQ(renderer.GetBackend(), RendererBackend::kVulkan);
}

TEST(VulkanRendererTest, DrawsTriangleMeshWithoutError) {
  SkipIfVulkanUnavailable();

  {
    VulkanRenderer renderer;
    renderer.CreateContext(WindowConfig{800, 600, "test"});

    EXPECT_NO_THROW(renderer.Render());
    EXPECT_NO_THROW(renderer.Render());
  }

  glfwTerminate();
}

}  // namespace
}  // namespace skeleton
