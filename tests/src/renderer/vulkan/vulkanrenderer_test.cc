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

TEST(VulkanRendererTest, FrameSubmitCallbackCanAddCommandBuffer) {
  SkipIfVulkanUnavailable();

  {
    VulkanRenderer renderer;
    renderer.CreateContext(WindowConfig{800, 600, "test"});

    VkCommandPool command_pool = VK_NULL_HANDLE;
    VkCommandPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = renderer.QueueFamilyIndex();
    ASSERT_EQ(
        vkCreateCommandPool(renderer.Device(), &pool_info, nullptr,
                            &command_pool),
        VK_SUCCESS);

    VkCommandBuffer command_buffer = VK_NULL_HANDLE;
    VkCommandBufferAllocateInfo allocate_info{};
    allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocate_info.commandPool = command_pool;
    allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocate_info.commandBufferCount = 1;
    ASSERT_EQ(vkAllocateCommandBuffers(renderer.Device(), &allocate_info,
                                       &command_buffer),
              VK_SUCCESS);

    uint32_t callback_invocation_count = 0;
    renderer.SetFrameSubmitCallback(
        [&callback_invocation_count, &command_buffer](uint32_t image_index) {
          ++callback_invocation_count;
          VkCommandBufferBeginInfo begin_info{};
          begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
          begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
          EXPECT_EQ(vkBeginCommandBuffer(command_buffer, &begin_info),
                    VK_SUCCESS);
          EXPECT_EQ(vkEndCommandBuffer(command_buffer), VK_SUCCESS);
          return command_buffer;
        });

    EXPECT_NO_THROW(renderer.Render());
    EXPECT_NO_THROW(renderer.Render());
    EXPECT_EQ(callback_invocation_count, 2);
    EXPECT_GE(renderer.SwapchainImageCount(), 1u);

    vkDeviceWaitIdle(renderer.Device());
    vkDestroyCommandPool(renderer.Device(), command_pool, nullptr);
  }

  glfwTerminate();
}

TEST(VulkanRendererTest, RendersToRenderTargetWithoutError) {
  SkipIfVulkanUnavailable();

  {
    VulkanRenderer renderer(RenderTarget::kRenderTargetTexture);
    renderer.CreateContext(WindowConfig{800, 600, "test"});

    EXPECT_NE(renderer.RenderTargetImageView(), VK_NULL_HANDLE);
    EXPECT_EQ(renderer.RenderTargetExtent().width, 800u);
    EXPECT_EQ(renderer.RenderTargetExtent().height, 600u);
    EXPECT_NO_THROW(renderer.Render());
    EXPECT_NO_THROW(renderer.Render());
  }

  glfwTerminate();
}

TEST(VulkanRendererTest, ResizeRenderTargetRecreatesImage) {
  SkipIfVulkanUnavailable();

  {
    VulkanRenderer renderer(RenderTarget::kRenderTargetTexture);
    renderer.CreateContext(WindowConfig{800, 600, "test"});
    const VkImageView original_image_view = renderer.RenderTargetImageView();

    renderer.ResizeRenderTarget(640, 480);

    EXPECT_NE(renderer.RenderTargetImageView(), VK_NULL_HANDLE);
    EXPECT_NE(renderer.RenderTargetImageView(), original_image_view);
    EXPECT_EQ(renderer.RenderTargetExtent().width, 640u);
    EXPECT_EQ(renderer.RenderTargetExtent().height, 480u);
    EXPECT_NO_THROW(renderer.Render());
  }

  glfwTerminate();
}

TEST(VulkanRendererTest, WindowTargetHasNoRenderTargetImage) {
  SkipIfVulkanUnavailable();

  {
    VulkanRenderer renderer;
    renderer.CreateContext(WindowConfig{800, 600, "test"});

    EXPECT_EQ(renderer.RenderTargetImageView(), VK_NULL_HANDLE);
    EXPECT_EQ(renderer.RenderTargetExtent().width, 0u);
    EXPECT_EQ(renderer.RenderTargetExtent().height, 0u);
  }

  glfwTerminate();
}

}  // namespace
}  // namespace skeleton
