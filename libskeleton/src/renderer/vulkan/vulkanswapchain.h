// Copyright 2026 aljabari

#ifndef LIBSKELETON_SRC_RENDERER_VULKAN_VULKANSWAPCHAIN_H_
#define LIBSKELETON_SRC_RENDERER_VULKAN_VULKANSWAPCHAIN_H_

#include <volk.h>

#include <cstdint>
#include <vector>

#include "renderer/vulkan/vulkandevice.h"

namespace skeleton {

// RAII wrapper around a Vulkan swapchain and the image views for its images.
// Construction picks a surface format and present mode, creates the swapchain
// sized for |width| x |height|, creates an image view for every image, and
// throws RendererCreationException on failure; destruction destroys the image
// views before the swapchain.
class VulkanSwapchain {
 public:
  VulkanSwapchain(const VulkanDevice& device, VkSurfaceKHR surface,
                  uint32_t width, uint32_t height);
  ~VulkanSwapchain();

  VulkanSwapchain(const VulkanSwapchain&) = delete;
  VulkanSwapchain& operator=(const VulkanSwapchain&) = delete;

  VkSwapchainKHR Swapchain() const;
  VkExtent2D Extent() const;
  VkFormat ImageFormat() const;
  // The minImageCount the swapchain was created with. The swapchain may own
  // more images than this.
  uint32_t MinImageCount() const;
  const std::vector<VkImage>& Images() const;
  const std::vector<VkImageView>& ImageViews() const;

 private:
  VkSurfaceFormatKHR ChooseSurfaceFormat() const;
  VkPresentModeKHR ChoosePresentMode() const;
  VkExtent2D ChooseExtent(uint32_t width, uint32_t height) const;
  void CreateSwapchain(uint32_t width, uint32_t height);
  void CreateImageViews();

  const VulkanDevice& device_;
  VkSurfaceKHR surface_ = VK_NULL_HANDLE;
  VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
  VkExtent2D extent_{};
  VkFormat image_format_ = VK_FORMAT_UNDEFINED;
  uint32_t min_image_count_ = 0;
  std::vector<VkImage> images_;
  std::vector<VkImageView> image_views_;
};

}  // namespace skeleton

#endif  // LIBSKELETON_SRC_RENDERER_VULKAN_VULKANSWAPCHAIN_H_
