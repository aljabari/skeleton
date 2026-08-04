// Copyright 2026 aljabari

#include "renderer/vulkan/vulkanmesh.h"

#include <GLFW/glfw3.h>
#include <gtest/gtest.h>

#include <vector>

#include "renderer/vulkan/vulkandevice.h"
#include "renderer/vulkan/vulkaninstance.h"

namespace skeleton {
namespace {

// The same hardcoded triangle mesh the OpenGL renderer tests use, authored in
// the Vulkan coordinate system: three vertices of interleaved position (vec3)
// and colour (vec3).
const std::vector<float> kTriangleVertices = {
    -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  //
    0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f,  //
    0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  //
};

// Initialises GLFW and skips the test when Vulkan is not available on the
// current system.
void SkipIfVulkanUnavailable() {
  ASSERT_TRUE(glfwInit());
  if (!glfwVulkanSupported()) {
    glfwTerminate();
    GTEST_SKIP() << "Vulkan not supported on this system";
  }
}

TEST(VulkanMeshTest, CreatesVertexBufferFromTriangleMesh) {
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
    const VulkanMesh mesh(device, kTriangleVertices);

    EXPECT_NE(mesh.Buffer(), VK_NULL_HANDLE);
    EXPECT_EQ(mesh.VertexCount(), 3u);
  }

  vkDestroySurfaceKHR(instance.Instance(), surface, nullptr);
  glfwDestroyWindow(window);
  glfwTerminate();
}

}  // namespace
}  // namespace skeleton
