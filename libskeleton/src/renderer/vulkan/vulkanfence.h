// Copyright 2026 aljabari

#ifndef LIBSKELETON_SRC_RENDERER_VULKAN_VULKANFENCE_H_
#define LIBSKELETON_SRC_RENDERER_VULKAN_VULKANFENCE_H_

#include <volk.h>

#include "renderer/vulkan/vulkandevice.h"

namespace skeleton {

// RAII wrapper around a single Vulkan fence. Construction throws
// RendererCreationException on failure; destruction destroys the fence.
class VulkanFence {
 public:
  explicit VulkanFence(const VulkanDevice& device);
  ~VulkanFence();

  VulkanFence(const VulkanFence&) = delete;
  VulkanFence& operator=(const VulkanFence&) = delete;

  VkFence Fence() const;

 private:
  const VulkanDevice& device_;
  VkFence fence_ = VK_NULL_HANDLE;
};

}  // namespace skeleton

#endif  // LIBSKELETON_SRC_RENDERER_VULKAN_VULKANFENCE_H_
