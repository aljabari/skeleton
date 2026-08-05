// Copyright 2026 aljabari

#ifndef LIBSKELETON_SRC_RENDERER_VULKAN_VULKANRENDERPASS_H_
#define LIBSKELETON_SRC_RENDERER_VULKAN_VULKANRENDERPASS_H_

#include <volk.h>

#include "renderer/vulkan/vulkandevice.h"

namespace skeleton {

// RAII wrapper around a Vulkan render pass for rendering the triangle mesh.
// The render pass has a single colour attachment (matching the swapchain image
// format) that is cleared at the start of the render pass and left in
// |final_layout| at its end. The swapchain scene pass uses the
// presentation-ready layout; the off-screen render target pass uses the
// shader-read-only layout so a shader can sample the rendered image.
// Construction throws RendererCreationException on failure; destruction
// destroys the render pass.
class VulkanRenderPass {
 public:
  VulkanRenderPass(const VulkanDevice& device, VkFormat color_format,
                   VkImageLayout final_layout);
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
