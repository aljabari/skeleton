// Copyright 2026 aljabari

#include "renderer/vulkan/vulkaninstance.h"

#include <spdlog/spdlog.h>

#include <memory>
#include <vector>

#include "libskeleton/renderer.h"

namespace skeleton {

namespace {

constexpr char kApplicationName[] = "Skeleton";
constexpr char kEngineName[] = "Skeleton";

}  // namespace

VulkanInstance::VulkanInstance(
    const std::vector<const char*>& instance_extensions) {
  if (volkInitialize() != VK_SUCCESS) {
    SPDLOG_ERROR("Failed to initialise the Vulkan loader.");
    throw RendererCreationException("Failed to initialise the Vulkan loader.");
  }

#if SKELETON_VULKAN_ENABLE_VALIDATION
  const bool validation_enabled = VulkanValidation::IsLayerAvailable();
  if (!validation_enabled) {
    SPDLOG_WARN("Vulkan validation layer {} not available.",
                VulkanValidation::kLayerName);
  }
#endif  // SKELETON_VULKAN_ENABLE_VALIDATION

  std::vector<const char*> extension_names = instance_extensions;
#if SKELETON_VULKAN_ENABLE_VALIDATION
  if (validation_enabled) {
    extension_names.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }
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
  create_info.enabledExtensionCount =
      static_cast<uint32_t>(extension_names.size());
  create_info.ppEnabledExtensionNames = extension_names.data();

#if SKELETON_VULKAN_ENABLE_VALIDATION
  if (validation_enabled) {
    const char* layer_names[] = {VulkanValidation::kLayerName};
    create_info.enabledLayerCount = 1;
    create_info.ppEnabledLayerNames = layer_names;
  }
#endif  // SKELETON_VULKAN_ENABLE_VALIDATION

  if (vkCreateInstance(&create_info, nullptr, &instance_) != VK_SUCCESS) {
    SPDLOG_ERROR("Failed to create the Vulkan instance.");
    throw RendererCreationException("Failed to create the Vulkan instance.");
  }
  volkLoadInstance(instance_);
#if SKELETON_VULKAN_ENABLE_VALIDATION
  if (validation_enabled) {
    validation_ = std::make_unique<VulkanValidation>(instance_);
  }
#endif  // SKELETON_VULKAN_ENABLE_VALIDATION
  SPDLOG_DEBUG("Created Vulkan instance.");
}

VulkanInstance::~VulkanInstance() {
#if SKELETON_VULKAN_ENABLE_VALIDATION
  validation_.reset();
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
  return validation_ ? validation_->Messenger() : VK_NULL_HANDLE;
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

}  // namespace skeleton
