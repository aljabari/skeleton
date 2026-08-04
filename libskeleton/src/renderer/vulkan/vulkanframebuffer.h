// Copyright 2026 aljabari

#ifndef LIBSKELETON_SRC_RENDERER_VULKAN_VULKANFRAMEBUFFER_H_
#define LIBSKELETON_SRC_RENDERER_VULKAN_VULKANFRAMEBUFFER_H_

#include <volk.h>

#include "renderer/vulkan/vulkandevice.h"

namespace skeleton {

// RAII wrapper around a single Vulkan framebuffer attaching one image view
// (one of the swapchain's) to a render pass at the given extent. Construction
// throws RendererCreationException on failure; destruction destroys the
// framebuffer.
class VulkanFramebuffer {
 public:
  VulkanFramebuffer(const VulkanDevice& device, VkRenderPass render_pass,
                    VkImageView image_view, VkExtent2D extent);
  ~VulkanFramebuffer();

  VulkanFramebuffer(const VulkanFramebuffer&) = delete;
  VulkanFramebuffer& operator=(const VulkanFramebuffer&) = delete;

  VkFramebuffer Framebuffer() const;

 private:
  const VulkanDevice& device_;
  VkFramebuffer framebuffer_ = VK_NULL_HANDLE;
};

}  // namespace skeleton

#endif  // LIBSKELETON_SRC_RENDERER_VULKAN_VULKANFRAMEBUFFER_H_
