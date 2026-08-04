// Copyright 2026 aljabari

#include "renderer/vulkan/vulkanvalidation.h"

#include <spdlog/spdlog.h>

#include <cstring>
#include <vector>

namespace skeleton {

#if SKELETON_VULKAN_ENABLE_VALIDATION
VulkanValidation::VulkanValidation(VkInstance instance) : instance_(instance) {
  CreateDebugMessenger();
}

VulkanValidation::~VulkanValidation() {
  if (messenger_ != VK_NULL_HANDLE) {
    vkDestroyDebugUtilsMessengerEXT(instance_, messenger_, nullptr);
  }
}

bool VulkanValidation::IsLayerAvailable() {
  uint32_t layer_count = 0;
  if (vkEnumerateInstanceLayerProperties(&layer_count, nullptr) != VK_SUCCESS) {
    return false;
  }
  std::vector<VkLayerProperties> layers(layer_count);
  vkEnumerateInstanceLayerProperties(&layer_count, layers.data());
  for (const VkLayerProperties& layer : layers) {
    if (std::strcmp(layer.layerName, kLayerName) == 0) {
      return true;
    }
  }
  return false;
}

VkDebugUtilsMessengerEXT VulkanValidation::Messenger() const {
  return messenger_;
}

void VulkanValidation::CreateDebugMessenger() {
  VkDebugUtilsMessengerCreateInfoEXT create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  create_info.messageSeverity =
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  create_info.pfnUserCallback = &VulkanValidation::DebugCallback;

  if (vkCreateDebugUtilsMessengerEXT(instance_, &create_info, nullptr,
                                     &messenger_) != VK_SUCCESS) {
    SPDLOG_WARN("Failed to create the Vulkan debug messenger.");
    return;
  }
  SPDLOG_INFO("Vulkan validation layer enabled.");
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanValidation::DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data) {
  (void)type;
  (void)user_data;
  switch (severity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
      SPDLOG_ERROR(callback_data->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
      SPDLOG_WARN(callback_data->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
      SPDLOG_INFO(callback_data->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
      SPDLOG_DEBUG(callback_data->pMessage);
      break;
    default:
      break;
  }
  return VK_FALSE;
}
#endif  // SKELETON_VULKAN_ENABLE_VALIDATION

}  // namespace skeleton
