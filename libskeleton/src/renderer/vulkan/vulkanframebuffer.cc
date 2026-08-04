// Copyright 2026 aljabari

#include "renderer/vulkan/vulkanframebuffer.h"

#include <spdlog/spdlog.h>

#include "libskeleton/renderer.h"

namespace skeleton {

VulkanFramebuffer::VulkanFramebuffer(const VulkanDevice& device,
                                     VkRenderPass render_pass,
                                     VkImageView image_view, VkExtent2D extent)
    : device_(device) {
  VkFramebufferCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  create_info.renderPass = render_pass;
  create_info.attachmentCount = 1;
  create_info.pAttachments = &image_view;
  create_info.width = extent.width;
  create_info.height = extent.height;
  create_info.layers = 1;

  if (vkCreateFramebuffer(device_.Device(), &create_info, nullptr,
                          &framebuffer_) != VK_SUCCESS) {
    SPDLOG_ERROR("Failed to create the Vulkan framebuffer.");
    throw RendererCreationException("Failed to create the Vulkan framebuffer.");
  }
  SPDLOG_DEBUG("Created Vulkan framebuffer ({}x{}).", extent.width,
               extent.height);
}

VulkanFramebuffer::~VulkanFramebuffer() {
  if (framebuffer_ != VK_NULL_HANDLE) {
    vkDestroyFramebuffer(device_.Device(), framebuffer_, nullptr);
  }
}

VkFramebuffer VulkanFramebuffer::Framebuffer() const {
  return framebuffer_;
}

}  // namespace skeleton
