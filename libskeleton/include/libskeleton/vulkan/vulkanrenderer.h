// Copyright 2026 aljabari

#ifndef LIBSKELETON_VULKAN_VULKANRENDERER_H_
#define LIBSKELETON_VULKAN_VULKANRENDERER_H_

#include <volk.h>

#include <memory>

#include "libskeleton/renderer.h"

struct GLFWwindow;

namespace skeleton {

class VulkanDevice;
class VulkanInstance;

class VulkanRenderer : public Renderer {
 public:
  VulkanRenderer();
  ~VulkanRenderer() override;

  RendererBackend GetBackend() const override;
  void SetWindowHints() override;
  void InitialiseForWindow(GLFWwindow* window) override;
  void Render() override;

  // The instance is created by the constructor; the surface, physical device,
  // logical device, and queues are created by InitialiseForWindow. Handles are
  // VK_NULL_HANDLE until their creation step succeeds, and the steps throw
  // RendererCreationException on failure.
  VkInstance Instance() const;
  VkSurfaceKHR Surface() const;
  VkPhysicalDevice PhysicalDevice() const;
  VkDevice Device() const;
  VkQueue GraphicsQueue() const;
  VkQueue PresentQueue() const;

 private:
  std::unique_ptr<VulkanInstance> instance_;
  std::unique_ptr<VulkanDevice> device_;
  VkSurfaceKHR surface_ = VK_NULL_HANDLE;
};

}  // namespace skeleton

#endif  // LIBSKELETON_VULKAN_VULKANRENDERER_H_
