// Copyright 2026 aljabari

#include "renderer/vulkan/vulkaninstance.h"

#include <spdlog/spdlog.h>

#include <cstring>
#include <vector>

#include "libskeleton/renderer.h"

namespace skeleton {

namespace {

constexpr char kApplicationName[] = "Skeleton";
constexpr char kEngineName[] = "Skeleton";

#if SKELETON_VULKAN_ENABLE_VALIDATION
constexpr char kValidationLayerName[] = "VK_LAYER_KHRONOS_validation";

bool IsValidationLayerAvailable() {
  uint32_t layer_count = 0;
  if (vkEnumerateInstanceLayerProperties(&layer_count, nullptr) != VK_SUCCESS) {
    return false;
  }
  std::vector<VkLayerProperties> layers(layer_count);
  vkEnumerateInstanceLayerProperties(&layer_count, layers.data());
  for (const VkLayerProperties& layer : layers) {
    if (std::strcmp(layer.layerName, kValidationLayerName) == 0) {
      return true;
    }
  }
  return false;
}
#endif  // SKELETON_VULKAN_ENABLE_VALIDATION

}  // namespace

VulkanInstance::VulkanInstance() {
  if (volkInitialize() != VK_SUCCESS) {
    SPDLOG_ERROR("Failed to initialise the Vulkan loader.");
    throw RendererCreationException("Failed to initialise the Vulkan loader.");
  }

#if SKELETON_VULKAN_ENABLE_VALIDATION
  const char* layer_names[] = {kValidationLayerName};
  const char* extension_names[] = {VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
  const bool validation_enabled = IsValidationLayerAvailable();
#endif  // SKELETON_VULKAN_ENABLE_VALIDATION

  VkApplicationInfo app_info{};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = kApplicationName;
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.pEngineName = kEngineName;
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &app_info;

#if SKELETON_VULKAN_ENABLE_VALIDATION
  if (validation_enabled) {
    create_info.enabledLayerCount = 1;
    create_info.ppEnabledLayerNames = layer_names;
    create_info.enabledExtensionCount = 1;
    create_info.ppEnabledExtensionNames = extension_names;
  } else {
    SPDLOG_WARN("Vulkan validation layer {} not available.",
                kValidationLayerName);
  }
#endif  // SKELETON_VULKAN_ENABLE_VALIDATION

  if (vkCreateInstance(&create_info, nullptr, &instance_) != VK_SUCCESS) {
    SPDLOG_ERROR("Failed to create the Vulkan instance.");
    throw RendererCreationException("Failed to create the Vulkan instance.");
  }
  volkLoadInstance(instance_);
#if SKELETON_VULKAN_ENABLE_VALIDATION
  if (validation_enabled) {
    CreateDebugMessenger();
  }
#endif  // SKELETON_VULKAN_ENABLE_VALIDATION
  SPDLOG_DEBUG("Created Vulkan instance.");
}

VulkanInstance::~VulkanInstance() {
#if SKELETON_VULKAN_ENABLE_VALIDATION
  if (debug_messenger_ != VK_NULL_HANDLE) {
    vkDestroyDebugUtilsMessengerEXT(instance_, debug_messenger_, nullptr);
  }
#endif  // SKELETON_VULKAN_ENABLE_VALIDATION
  if (instance_ != VK_NULL_HANDLE) {
    vkDestroyInstance(instance_, nullptr);
  }
}

VkInstance VulkanInstance::Instance() const {
  return instance_;
}

#if SKELETON_VULKAN_ENABLE_VALIDATION
VkDebugUtilsMessengerEXT VulkanInstance::DebugMessenger() const {
  return debug_messenger_;
}
#endif  // SKELETON_VULKAN_ENABLE_VALIDATION

std::vector<VkPhysicalDevice> VulkanInstance::EnumeratePhysicalDevices() const {
  uint32_t device_count = 0;
  if (vkEnumeratePhysicalDevices(instance_, &device_count, nullptr) !=
          VK_SUCCESS ||
      device_count == 0) {
    SPDLOG_ERROR("No Vulkan physical devices found.");
    throw RendererCreationException("No Vulkan physical devices found.");
  }

  std::vector<VkPhysicalDevice> devices(device_count);
  vkEnumeratePhysicalDevices(instance_, &device_count, devices.data());
  return devices;
}

#if SKELETON_VULKAN_ENABLE_VALIDATION
void VulkanInstance::CreateDebugMessenger() {
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
  create_info.pfnUserCallback = &VulkanInstance::DebugCallback;

  if (vkCreateDebugUtilsMessengerEXT(instance_, &create_info, nullptr,
                                     &debug_messenger_) != VK_SUCCESS) {
    SPDLOG_WARN("Failed to create the Vulkan debug messenger.");
    return;
  }
  SPDLOG_INFO("Vulkan validation layer enabled.");
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanInstance::DebugCallback(
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
