// Copyright 2026 aljabari

#ifndef LIBSKELETON_SRC_RENDERER_VULKAN_VULKANRENDERPASS_H_
#define LIBSKELETON_SRC_RENDERER_VULKAN_VULKANRENDERPASS_H_

#include <volk.h>

#include "renderer/vulkan/vulkandevice.h"

namespace skeleton {

// RAII wrapper around a Vulkan render pass for rendering the triangle mesh.
// The render pass has a single colour attachment (matching the swapchain image
// format) that is cleared at the start of the render pass and left in the
// presentation-ready layout at its end. Construction throws
// RendererCreationException on failure; destruction destroys the render pass.
class VulkanRenderPass {
 public:
  VulkanRenderPass(const VulkanDevice& device, VkFormat color_format);
  ~VulkanRenderPass();

  VulkanRenderPass(const VulkanRenderPass&) = delete;
  VulkanRenderPass& operator=(const VulkanRenderPass&) = delete;

  VkRenderPass RenderPass() const;

 private:
  const VulkanDevice& device_;
  VkRenderPass render_pass_ = VK_NULL_HANDLE;
};

}  // namespace skeleton

#endif  // LIBSKELETON_SRC_RENDERER_VULKAN_VULKANRENDERPASS_H_
