// Copyright 2026 aljabari

#ifndef LIBSKELETON_SRC_RENDERER_VULKAN_VULKANINSTANCE_H_
#define LIBSKELETON_SRC_RENDERER_VULKAN_VULKANINSTANCE_H_

#include <volk.h>

#include <memory>
#include <vector>

#if SKELETON_VULKAN_ENABLE_VALIDATION
#include "renderer/vulkan/vulkanvalidation.h"
#endif  // SKELETON_VULKAN_ENABLE_VALIDATION

namespace skeleton {

// RAII wrapper around a Vulkan instance. Construction initialises the volk
// loader, creates the instance, and throws RendererCreationException on
// failure; destruction destroys the instance. |instance_extensions| lists the
// instance extension names to enable (e.g. the surface extensions required by
// the windowing system). In builds that define
// SKELETON_VULKAN_ENABLE_VALIDATION the instance additionally requests the
// VK_LAYER_KHRONOS_validation layer and a VK_EXT_debug_utils messenger via
// VulkanValidation, whose callback routes validation messages into the
// project logger.
class VulkanInstance {
 public:
  explicit VulkanInstance(
      const std::vector<const char*>& instance_extensions = {});
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
  VkInstance instance_ = VK_NULL_HANDLE;
#if SKELETON_VULKAN_ENABLE_VALIDATION
  std::unique_ptr<VulkanValidation> validation_;
#endif  // SKELETON_VULKAN_ENABLE_VALIDATION
};

}  // namespace skeleton

#endif  // LIBSKELETON_SRC_RENDERER_VULKAN_VULKANINSTANCE_H_
