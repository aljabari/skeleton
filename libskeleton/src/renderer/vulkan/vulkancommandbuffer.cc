// Copyright 2026 aljabari

#include "renderer/vulkan/vulkancommandbuffer.h"

#include <spdlog/spdlog.h>

#include "libskeleton/renderer.h"

namespace skeleton {

VulkanCommandBuffer::VulkanCommandBuffer(const VulkanDevice& device)
    : device_(device) {
  VkCommandPoolCreateInfo pool_info{};
  pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  pool_info.queueFamilyIndex = device_.QueueFamilyIndex();

  if (vkCreateCommandPool(device_.Device(), &pool_info, nullptr,
                          &command_pool_) != VK_SUCCESS) {
    SPDLOG_ERROR("Failed to create the Vulkan command pool.");
    throw RendererCreationException(
        "Failed to create the Vulkan command pool.");
  }

  VkCommandBufferAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.commandPool = command_pool_;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandBufferCount = 1;

  if (vkAllocateCommandBuffers(device_.Device(), &alloc_info,
                               &command_buffer_) != VK_SUCCESS) {
    vkDestroyCommandPool(device_.Device(), command_pool_, nullptr);
    SPDLOG_ERROR("Failed to allocate the Vulkan command buffer.");
    throw RendererCreationException(
        "Failed to allocate the Vulkan command buffer.");
  }
  SPDLOG_DEBUG("Created Vulkan command buffer.");
}

VulkanCommandBuffer::~VulkanCommandBuffer() {
  if (command_pool_ != VK_NULL_HANDLE) {
    // Destroying the pool frees the command buffers allocated from it.
    vkDestroyCommandPool(device_.Device(), command_pool_, nullptr);
  }
}

VkCommandPool VulkanCommandBuffer::CommandPool() const {
  return command_pool_;
}

VkCommandBuffer VulkanCommandBuffer::CommandBuffer() const {
  return command_buffer_;
}

}  // namespace skeleton
