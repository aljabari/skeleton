// Copyright 2026 aljabari

#include "renderer/vulkan/vulkangraphicspipeline.h"

#include <GLFW/glfw3.h>
#include <gtest/gtest.h>

#include <string>
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

// Creates a minimal render pass with a single colour attachment in the
// presentation-ready final layout, matching what the pipeline renders into.
void CreateColorRenderPass(VkDevice device, VkFormat format,
                           VkRenderPass* out_render_pass) {
  VkAttachmentDescription attachment{};
  attachment.format = format;
  attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference color_reference{};
  color_reference.attachment = 0;
  color_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &color_reference;

  VkRenderPassCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  create_info.attachmentCount = 1;
  create_info.pAttachments = &attachment;
  create_info.subpassCount = 1;
  create_info.pSubpasses = &subpass;

  VkRenderPass render_pass = VK_NULL_HANDLE;
  ASSERT_EQ(vkCreateRenderPass(device, &create_info, nullptr, &render_pass),
            VK_SUCCESS);
  *out_render_pass = render_pass;
}

TEST(VulkanGraphicsPipelineTest, CreatesPipelineFromCompiledShaders) {
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
    VkRenderPass render_pass = VK_NULL_HANDLE;
    CreateColorRenderPass(device.Device(), VK_FORMAT_B8G8R8A8_UNORM,
                          &render_pass);

    const std::string shader_directory = SKELETON_SHADER_DIR;
    {
      VulkanGraphicsPipeline pipeline(
          device, render_pass, shader_directory + "/triangle.vert.spv",
          shader_directory + "/triangle.frag.spv");

      EXPECT_NE(pipeline.Pipeline(), VK_NULL_HANDLE);
      EXPECT_NE(pipeline.Layout(), VK_NULL_HANDLE);
    }

    vkDestroyRenderPass(device.Device(), render_pass, nullptr);
  }

  vkDestroySurfaceKHR(instance.Instance(), surface, nullptr);
  glfwDestroyWindow(window);
  glfwTerminate();
}

}  // namespace
}  // namespace skeleton
