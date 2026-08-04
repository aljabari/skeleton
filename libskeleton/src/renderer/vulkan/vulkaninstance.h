// Copyright 2026 aljabari

#ifndef LIBSKELETON_SRC_RENDERER_VULKAN_VULKANINSTANCE_H_
#define LIBSKELETON_SRC_RENDERER_VULKAN_VULKANINSTANCE_H_

#include <volk.h>

#include <vector>

namespace skeleton {

// RAII wrapper around a Vulkan instance. Construction initialises the volk
// loader, creates the instance, and throws RendererCreationException on
// failure; destruction destroys the instance. In builds that define
// SKELETON_VULKAN_ENABLE_VALIDATION the instance requests the
// VK_LAYER_KHRONOS_validation layer and a VK_EXT_debug_utils messenger whose
// callback routes validation messages into the project logger.
class VulkanInstance {
 public:
  VulkanInstance();
  ~VulkanInstance();

  VulkanInstance(const VulkanInstance&) = delete;
  VulkanInstance& operator=(const VulkanInstance&) = delete;

  VkInstance Instance() const;

#if SKELETON_VULKAN_ENABLE_VALIDATION
  // Handle of the debug messenger receiving validation messages, or
  // VK_NULL_HANDLE when the validation layer is not available.
  VkDebugUtilsMessengerEXT DebugMessenger() const;
#endif  // SKELETON_VULKAN_ENABLE_VALIDATION

  // Returns the physical devices available on the system. Throws
  // RendererCreationException when none can be enumerated.
  std::vector<VkPhysicalDevice> EnumeratePhysicalDevices() const;

 private:
#if SKELETON_VULKAN_ENABLE_VALIDATION
  // Logs a validation message through spdlog, keyed by message severity.
  static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
      VkDebugUtilsMessageSeverityFlagBitsEXT severity,
      VkDebugUtilsMessageTypeFlagsEXT type,
      const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
      void* user_data);
  void CreateDebugMessenger();
#endif  // SKELETON_VULKAN_ENABLE_VALIDATION

  VkInstance instance_ = VK_NULL_HANDLE;
#if SKELETON_VULKAN_ENABLE_VALIDATION
  VkDebugUtilsMessengerEXT debug_messenger_ = VK_NULL_HANDLE;
#endif  // SKELETON_VULKAN_ENABLE_VALIDATION
};

}  // namespace skeleton

#endif  // LIBSKELETON_SRC_RENDERER_VULKAN_VULKANINSTANCE_H_
