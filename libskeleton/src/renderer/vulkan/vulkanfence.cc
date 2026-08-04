// Copyright 2026 aljabari

#include "renderer/vulkan/vulkanfence.h"

#include <spdlog/spdlog.h>

#include "libskeleton/renderer.h"

namespace skeleton {

VulkanFence::VulkanFence(const VulkanDevice& device) : device_(device) {
  VkFenceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

  if (vkCreateFence(device_.Device(), &create_info, nullptr, &fence_) !=
      VK_SUCCESS) {
    SPDLOG_ERROR("Failed to create the Vulkan fence.");
    throw RendererCreationException("Failed to create the Vulkan fence.");
  }
  SPDLOG_DEBUG("Created Vulkan fence.");
}

VulkanFence::~VulkanFence() {
  if (fence_ != VK_NULL_HANDLE) {
    vkDestroyFence(device_.Device(), fence_, nullptr);
  }
}

VkFence VulkanFence::Fence() const {
  return fence_;
}

}  // namespace skeleton
