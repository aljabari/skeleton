// Copyright 2026 aljabari

#ifndef LIBSKELETON_SRC_RENDERER_VULKAN_VULKANDEVICE_H_
#define LIBSKELETON_SRC_RENDERER_VULKAN_VULKANDEVICE_H_

#include <volk.h>

#include <cstdint>

#include "renderer/vulkan/vulkaninstance.h"

namespace skeleton {

// RAII wrapper around a Vulkan logical device. Construction selects a physical
// device with a graphics queue family from |instance|, creates the logical
// device, and throws RendererCreationException on failure; destruction
// destroys the logical device.
class VulkanDevice {
 public:
  explicit VulkanDevice(const VulkanInstance& instance);
  ~VulkanDevice();

  VulkanDevice(const VulkanDevice&) = delete;
  VulkanDevice& operator=(const VulkanDevice&) = delete;

  VkPhysicalDevice PhysicalDevice() const;
  VkDevice Device() const;
  VkQueue GraphicsQueue() const;
  uint32_t QueueFamilyIndex() const;

 private:
  bool HasGraphicsQueueFamily(VkPhysicalDevice device,
                              uint32_t* queue_family_index) const;
  void CreateLogicalDevice();

  VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
  VkDevice device_ = VK_NULL_HANDLE;
  uint32_t queue_family_index_ = 0;
  VkQueue graphics_queue_ = VK_NULL_HANDLE;
};

}  // namespace skeleton

#endif  // LIBSKELETON_SRC_RENDERER_VULKAN_VULKANDEVICE_H_
