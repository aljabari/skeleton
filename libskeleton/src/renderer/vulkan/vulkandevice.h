// Copyright 2026 aljabari

#ifndef LIBSKELETON_SRC_RENDERER_VULKAN_VULKANDEVICE_H_
#define LIBSKELETON_SRC_RENDERER_VULKAN_VULKANDEVICE_H_

#include <volk.h>

#include <cstdint>

#include "renderer/vulkan/vulkaninstance.h"

namespace skeleton {

// RAII wrapper around a Vulkan logical device. Construction selects a physical
// device with a graphics queue family and a queue family that supports
// presentation to |surface| (possibly the same family), creates the logical
// device with a queue for each, and throws RendererCreationException on
// failure; destruction destroys the logical device.
class VulkanDevice {
 public:
  VulkanDevice(const VulkanInstance& instance, VkSurfaceKHR surface);
  ~VulkanDevice();

  VulkanDevice(const VulkanDevice&) = delete;
  VulkanDevice& operator=(const VulkanDevice&) = delete;

  VkPhysicalDevice PhysicalDevice() const;
  VkDevice Device() const;
  VkQueue GraphicsQueue() const;
  VkQueue PresentQueue() const;
  uint32_t QueueFamilyIndex() const;
  uint32_t PresentQueueFamilyIndex() const;

 private:
  bool FindQueueFamilies(VkPhysicalDevice device, uint32_t* graphics_index,
                         uint32_t* present_index) const;
  void CreateLogicalDevice();

  VkSurfaceKHR surface_ = VK_NULL_HANDLE;
  VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
  VkDevice device_ = VK_NULL_HANDLE;
  uint32_t queue_family_index_ = 0;
  uint32_t present_queue_family_index_ = 0;
  VkQueue graphics_queue_ = VK_NULL_HANDLE;
  VkQueue present_queue_ = VK_NULL_HANDLE;
};

}  // namespace skeleton

#endif  // LIBSKELETON_SRC_RENDERER_VULKAN_VULKANDEVICE_H_
