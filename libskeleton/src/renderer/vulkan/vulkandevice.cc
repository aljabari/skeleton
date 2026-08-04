// Copyright 2026 aljabari

#include "renderer/vulkan/vulkandevice.h"

#include <spdlog/spdlog.h>

#include <vector>

#include "libskeleton/renderer.h"

namespace skeleton {

VulkanDevice::VulkanDevice(const VulkanInstance& instance,
                           VkSurfaceKHR surface)
    : surface_(surface) {
  for (VkPhysicalDevice device : instance.EnumeratePhysicalDevices()) {
    uint32_t queue_family_index = 0;
    uint32_t present_queue_family_index = 0;
    if (FindQueueFamilies(device, &queue_family_index,
                          &present_queue_family_index)) {
      physical_device_ = device;
      queue_family_index_ = queue_family_index;
      present_queue_family_index_ = present_queue_family_index;
      CreateLogicalDevice();
      SPDLOG_DEBUG("Created Vulkan logical device.");
      return;
    }
  }
  SPDLOG_ERROR("No Vulkan physical device with a suitable queue found.");
  throw RendererCreationException(
      "No Vulkan physical device with a suitable queue found.");
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

VkQueue VulkanDevice::PresentQueue() const {
  return present_queue_;
}

uint32_t VulkanDevice::QueueFamilyIndex() const {
  return queue_family_index_;
}

uint32_t VulkanDevice::PresentQueueFamilyIndex() const {
  return present_queue_family_index_;
}

bool VulkanDevice::FindQueueFamilies(VkPhysicalDevice device,
                                     uint32_t* graphics_index,
                                     uint32_t* present_index) const {
  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
                                           nullptr);
  std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
                                           queue_families.data());
  bool has_graphics = false;
  bool has_present = false;
  for (uint32_t i = 0; i < queue_families.size(); ++i) {
    VkBool32 present_supported = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_,
                                         &present_supported);
    if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      *graphics_index = i;
      has_graphics = true;
    }
    if (present_supported == VK_TRUE) {
      *present_index = i;
      has_present = true;
    }
    if (has_graphics && has_present) {
      return true;
    }
  }
  return has_graphics && has_present;
}

void VulkanDevice::CreateLogicalDevice() {
  const float queue_priority = 1.0f;
  std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
  std::vector<uint32_t> queue_families = {queue_family_index_};
  if (present_queue_family_index_ != queue_family_index_) {
    queue_families.push_back(present_queue_family_index_);
  }
  for (uint32_t family : queue_families) {
    VkDeviceQueueCreateInfo queue_create_info{};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = family;
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &queue_priority;
    queue_create_infos.push_back(queue_create_info);
  }

  VkDeviceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create_info.queueCreateInfoCount = queue_create_infos.size();
  create_info.pQueueCreateInfos = queue_create_infos.data();
  const char* device_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  create_info.enabledExtensionCount = 1;
  create_info.ppEnabledExtensionNames = device_extensions;

  if (vkCreateDevice(physical_device_, &create_info, nullptr, &device_) !=
      VK_SUCCESS) {
    SPDLOG_ERROR("Failed to create the Vulkan logical device.");
    throw RendererCreationException(
        "Failed to create the Vulkan logical device.");
  }
  volkLoadDevice(device_);
  vkGetDeviceQueue(device_, queue_family_index_, 0, &graphics_queue_);
  vkGetDeviceQueue(device_, present_queue_family_index_, 0, &present_queue_);
}

}  // namespace skeleton
