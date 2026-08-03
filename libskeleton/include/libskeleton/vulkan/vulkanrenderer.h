// Copyright 2026 aljabari

#ifndef LIBSKELETON_VULKAN_VULKANRENDERER_H_
#define LIBSKELETON_VULKAN_VULKANRENDERER_H_

#include <volk.h>

#include <cstdint>

#include "libskeleton/renderer.h"

struct GLFWwindow;

namespace skeleton {

class VulkanRenderer : public Renderer {
 public:
  VulkanRenderer();
  ~VulkanRenderer() override;

  RendererBackend GetBackend() const override;
  void SetWindowHints() override;
  void InitialiseForWindow(GLFWwindow* window) override;
  void Render() override;

  // Vulkan handles created by the constructor. Only valid after a successful
  // construction; the constructor throws RendererCreationException on failure.
  VkInstance Instance() const;
  VkPhysicalDevice PhysicalDevice() const;
  VkDevice Device() const;
  VkQueue GraphicsQueue() const;

 private:
  void CreateInstance();
  void SelectPhysicalDevice();
  void CreateLogicalDevice();
  bool HasGraphicsQueueFamily(VkPhysicalDevice device);
  void Cleanup();

  VkInstance instance_ = VK_NULL_HANDLE;
  VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
  VkDevice device_ = VK_NULL_HANDLE;
  uint32_t graphics_queue_family_ = 0;
  VkQueue graphics_queue_ = VK_NULL_HANDLE;
};

}  // namespace skeleton

#endif  // LIBSKELETON_VULKAN_VULKANRENDERER_H_
