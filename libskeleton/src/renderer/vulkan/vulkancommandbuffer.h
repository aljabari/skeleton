// Copyright 2026 aljabari

#ifndef LIBSKELETON_SRC_RENDERER_VULKAN_VULKANCOMMANDBUFFER_H_
#define LIBSKELETON_SRC_RENDERER_VULKAN_VULKANCOMMANDBUFFER_H_

#include <volk.h>

#include "renderer/vulkan/vulkandevice.h"

namespace skeleton {

// RAII wrapper around a Vulkan command pool and one primary command buffer
// allocated from it. The pool is created for the device's graphics queue
// family and allows individual command buffers to be reset. Construction
// throws RendererCreationException on failure; destruction frees the command
// buffer before destroying the pool.
class VulkanCommandBuffer {
 public:
  explicit VulkanCommandBuffer(const VulkanDevice& device);
  ~VulkanCommandBuffer();

  VulkanCommandBuffer(const VulkanCommandBuffer&) = delete;
  VulkanCommandBuffer& operator=(const VulkanCommandBuffer&) = delete;

  VkCommandPool CommandPool() const;
  VkCommandBuffer CommandBuffer() const;

 private:
  const VulkanDevice& device_;
  VkCommandPool command_pool_ = VK_NULL_HANDLE;
  VkCommandBuffer command_buffer_ = VK_NULL_HANDLE;
};

}  // namespace skeleton

#endif  // LIBSKELETON_SRC_RENDERER_VULKAN_VULKANCOMMANDBUFFER_H_
