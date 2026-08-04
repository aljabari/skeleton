// Copyright 2026 aljabari

#ifndef LIBSKELETON_SRC_RENDERER_VULKAN_VULKANSEMAPHORE_H_
#define LIBSKELETON_SRC_RENDERER_VULKAN_VULKANSEMAPHORE_H_

#include <volk.h>

#include "renderer/vulkan/vulkandevice.h"

namespace skeleton {

// RAII wrapper around a single Vulkan semaphore. Construction throws
// RendererCreationException on failure; destruction destroys the semaphore.
class VulkanSemaphore {
 public:
  explicit VulkanSemaphore(const VulkanDevice& device);
  ~VulkanSemaphore();

  VulkanSemaphore(const VulkanSemaphore&) = delete;
  VulkanSemaphore& operator=(const VulkanSemaphore&) = delete;

  VkSemaphore Semaphore() const;

 private:
  const VulkanDevice& device_;
  VkSemaphore semaphore_ = VK_NULL_HANDLE;
};

}  // namespace skeleton

#endif  // LIBSKELETON_SRC_RENDERER_VULKAN_VULKANSEMAPHORE_H_
