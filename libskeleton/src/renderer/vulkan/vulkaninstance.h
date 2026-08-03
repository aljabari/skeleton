// Copyright 2026 aljabari

#ifndef LIBSKELETON_SRC_RENDERER_VULKAN_VULKANINSTANCE_H_
#define LIBSKELETON_SRC_RENDERER_VULKAN_VULKANINSTANCE_H_

#include <volk.h>

#include <vector>

namespace skeleton {

// RAII wrapper around a Vulkan instance. Construction initialises the volk
// loader, creates the instance, and throws RendererCreationException on
// failure; destruction destroys the instance.
class VulkanInstance {
 public:
  VulkanInstance();
  ~VulkanInstance();

  VulkanInstance(const VulkanInstance&) = delete;
  VulkanInstance& operator=(const VulkanInstance&) = delete;

  VkInstance Instance() const;

  // Returns the physical devices available on the system. Throws
  // RendererCreationException when none can be enumerated.
  std::vector<VkPhysicalDevice> EnumeratePhysicalDevices() const;

 private:
  VkInstance instance_ = VK_NULL_HANDLE;
};

}  // namespace skeleton

#endif  // LIBSKELETON_SRC_RENDERER_VULKAN_VULKANINSTANCE_H_
