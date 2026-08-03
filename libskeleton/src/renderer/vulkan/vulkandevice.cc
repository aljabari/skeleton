// Copyright 2026 aljabari

#include "renderer/vulkan/vulkandevice.h"

#include <vector>

#include "libskeleton/renderer.h"

namespace skeleton {

VulkanDevice::VulkanDevice(const VulkanInstance& instance) {
  for (VkPhysicalDevice device : instance.EnumeratePhysicalDevices()) {
    uint32_t queue_family_index = 0;
    if (HasGraphicsQueueFamily(device, &queue_family_index)) {
      physical_device_ = device;
      queue_family_index_ = queue_family_index;
      CreateLogicalDevice();
      return;
    }
  }
  throw RendererCreationException(
      "No Vulkan physical device with a graphics queue found.");
}

VulkanDevice::~VulkanDevice() {
  if (device_ != VK_NULL_HANDLE) {
    vkDestroyDevice(device_, nullptr);
  }
}

VkPhysicalDevice VulkanDevice::PhysicalDevice() const {
  return physical_device_;
}

VkDevice VulkanDevice::Device() const {
  return device_;
}

VkQueue VulkanDevice::GraphicsQueue() const {
  return graphics_queue_;
}

uint32_t VulkanDevice::QueueFamilyIndex() const {
  return queue_family_index_;
}

bool VulkanDevice::HasGraphicsQueueFamily(
    VkPhysicalDevice device, uint32_t* queue_family_index) const {
  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
                                           nullptr);
  std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
                                           queue_families.data());
  for (uint32_t i = 0; i < queue_families.size(); ++i) {
    if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      *queue_family_index = i;
      return true;
    }
  }
  return false;
}

void VulkanDevice::CreateLogicalDevice() {
  const float queue_priority = 1.0f;
  VkDeviceQueueCreateInfo queue_create_info{};
  queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queue_create_info.queueFamilyIndex = queue_family_index_;
  queue_create_info.queueCount = 1;
  queue_create_info.pQueuePriorities = &queue_priority;

  VkDeviceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create_info.queueCreateInfoCount = 1;
  create_info.pQueueCreateInfos = &queue_create_info;

  if (vkCreateDevice(physical_device_, &create_info, nullptr, &device_) !=
      VK_SUCCESS) {
    throw RendererCreationException(
        "Failed to create the Vulkan logical device.");
  }
  volkLoadDevice(device_);
  vkGetDeviceQueue(device_, queue_family_index_, 0, &graphics_queue_);
}

}  // namespace skeleton
