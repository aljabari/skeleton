// Copyright 2026 aljabari

#include "renderer/vulkan/vulkanrenderpass.h"

#include <spdlog/spdlog.h>

#include "libskeleton/renderer.h"

namespace skeleton {

VulkanRenderPass::VulkanRenderPass(const VulkanDevice& device,
                                   VkFormat color_format,
                                   VkImageLayout final_layout)
    : device_(device) {
  VkAttachmentDescription attachment{};
  attachment.format = color_format;
  attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  attachment.finalLayout = final_layout;

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

  if (vkCreateRenderPass(device_.Device(), &create_info, nullptr,
                         &render_pass_) != VK_SUCCESS) {
    SPDLOG_ERROR("Failed to create the Vulkan render pass.");
    throw RendererCreationException("Failed to create the Vulkan render pass.");
  }
  SPDLOG_DEBUG("Created Vulkan render pass.");
}

VulkanRenderPass::~VulkanRenderPass() {
  if (render_pass_ != VK_NULL_HANDLE) {
    vkDestroyRenderPass(device_.Device(), render_pass_, nullptr);
  }
}

VkRenderPass VulkanRenderPass::RenderPass() const {
  return render_pass_;
}

}  // namespace skeleton
