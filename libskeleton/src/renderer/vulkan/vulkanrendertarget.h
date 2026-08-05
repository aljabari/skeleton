// Copyright 2026 aljabari

#ifndef LIBSKELETON_SRC_RENDERER_VULKAN_VULKANRENDERTARGET_H_
#define LIBSKELETON_SRC_RENDERER_VULKAN_VULKANRENDERTARGET_H_

#include <volk.h>

#include <cstdint>

#include "renderer/vulkan/vulkandevice.h"

namespace skeleton {

// RAII wrapper around an off-screen Vulkan colour image and its image view.
// Construction creates a 2D image of |format| at |width| x |height| usable as
// both a colour attachment and a sampled texture, allocates device-local memory
// for it, and creates an image view over the whole image; destruction frees the
// memory and destroys the image view and image. The image starts in
// VK_IMAGE_LAYOUT_UNDEFINED; the render pass the renderer records over it
// clears it and leaves it in the shader-read-only layout at the end of each
// frame, so a shader (for example the editor's ImGui viewport image) can sample
// it. Construction throws RendererCreationException on failure.
class VulkanRenderTarget {
 public:
  VulkanRenderTarget(const VulkanDevice& device, VkFormat format,
                     uint32_t width, uint32_t height);
  ~VulkanRenderTarget();

  VulkanRenderTarget(const VulkanRenderTarget&) = delete;
  VulkanRenderTarget& operator=(const VulkanRenderTarget&) = delete;

  VkImage Image() const;
  VkImageView ImageView() const;
  VkExtent2D Extent() const;
  VkFormat Format() const;

 private:
  // Returns a memory type index satisfying |type_bits| and |properties|, or
  // throws RendererCreationException when none exists.
  uint32_t FindMemoryType(uint32_t type_bits,
                          VkMemoryPropertyFlags properties) const;
  void CreateImage();
  void CreateImageView();

  const VulkanDevice& device_;
  VkImage image_ = VK_NULL_HANDLE;
  VkDeviceMemory memory_ = VK_NULL_HANDLE;
  VkImageView image_view_ = VK_NULL_HANDLE;
  VkFormat format_ = VK_FORMAT_UNDEFINED;
  VkExtent2D extent_{};
};

}  // namespace skeleton

#endif  // LIBSKELETON_SRC_RENDERER_VULKAN_VULKANRENDERTARGET_H_
