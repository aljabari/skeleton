// Copyright 2026 aljabari

#ifndef LIBSKELETON_SRC_RENDERER_VULKAN_VULKANVALIDATION_H_
#define LIBSKELETON_SRC_RENDERER_VULKAN_VULKANVALIDATION_H_

#include <volk.h>

namespace skeleton {

#if SKELETON_VULKAN_ENABLE_VALIDATION
// RAII wrapper around a Vulkan debug messenger that routes validation
// messages into the project logger. Construction creates a messenger for
// |instance| and logs a warning when that fails; destruction destroys the
// messenger before the instance is destroyed.
class VulkanValidation {
 public:
  // Name of the validation layer requested from the instance.
  static constexpr char kLayerName[] = "VK_LAYER_KHRONOS_validation";

  // Returns true when the validation layer can be requested from the instance.
  static bool IsLayerAvailable();

  explicit VulkanValidation(VkInstance instance);
  ~VulkanValidation();

  VulkanValidation(const VulkanValidation&) = delete;
  VulkanValidation& operator=(const VulkanValidation&) = delete;

  VkDebugUtilsMessengerEXT Messenger() const;

 private:
  // Logs a validation message through spdlog, keyed by message severity.
  static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
      VkDebugUtilsMessageSeverityFlagBitsEXT severity,
      VkDebugUtilsMessageTypeFlagsEXT type,
      const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
      void* user_data);
  void CreateDebugMessenger();

  VkInstance instance_ = VK_NULL_HANDLE;
  VkDebugUtilsMessengerEXT messenger_ = VK_NULL_HANDLE;
};
#endif  // SKELETON_VULKAN_ENABLE_VALIDATION

}  // namespace skeleton

#endif  // LIBSKELETON_SRC_RENDERER_VULKAN_VULKANVALIDATION_H_
