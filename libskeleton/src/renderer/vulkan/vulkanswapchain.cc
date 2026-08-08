// Copyright 2026 aljabari

#include "renderer/vulkan/vulkanswapchain.h"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <cstdint>
#include <limits>
#include <vector>

#include "libskeleton/renderer.h"

namespace skeleton {

VulkanSwapchain::VulkanSwapchain(const VulkanDevice& device,
                                 VkSurfaceKHR surface, uint32_t width,
                                 uint32_t height)
    : device_(device), surface_(surface) {
  CreateSwapchain(width, height);
  CreateImageViews();
  SPDLOG_DEBUG("Created Vulkan swapchain ({}x{}).", extent_.width,
               extent_.height);
}

VulkanSwapchain::~VulkanSwapchain() {
  for (VkImageView image_view : image_views_) {
    vkDestroyImageView(device_.Device(), image_view, nullptr);
  }
  if (swapchain_ != VK_NULL_HANDLE) {
    vkDestroySwapchainKHR(device_.Device(), swapchain_, nullptr);
  }
}

VkSwapchainKHR VulkanSwapchain::Swapchain() const {
  return swapchain_;
}

VkExtent2D VulkanSwapchain::Extent() const {
  return extent_;
}

VkFormat VulkanSwapchain::ImageFormat() const {
  return image_format_;
}

uint32_t VulkanSwapchain::MinImageCount() const {
  return min_image_count_;
}

const std::vector<VkImage>& VulkanSwapchain::Images() const {
  return images_;
}

const std::vector<VkImageView>& VulkanSwapchain::ImageViews() const {
  return image_views_;
}

VkSurfaceFormatKHR VulkanSwapchain::ChooseSurfaceFormat() const {
  uint32_t format_count = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device_.PhysicalDevice(), surface_,
                                       &format_count, nullptr);
  std::vector<VkSurfaceFormatKHR> formats(format_count);
  vkGetPhysicalDeviceSurfaceFormatsKHR(device_.PhysicalDevice(), surface_,
                                       &format_count, formats.data());
  for (const VkSurfaceFormatKHR& format : formats) {
    if (format.format == VK_FORMAT_B8G8R8A8_UNORM &&
        format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return format;
    }
  }
  return formats.empty() ? VkSurfaceFormatKHR{} : formats[0];
}

VkPresentModeKHR VulkanSwapchain::ChoosePresentMode() const {
  uint32_t present_mode_count = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device_.PhysicalDevice(), surface_,
                                            &present_mode_count, nullptr);
  std::vector<VkPresentModeKHR> present_modes(present_mode_count);
  vkGetPhysicalDeviceSurfacePresentModesKHR(device_.PhysicalDevice(), surface_,
                                            &present_mode_count,
                                            present_modes.data());
  for (VkPresentModeKHR present_mode : present_modes) {
    if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return present_mode;
    }
  }
  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanSwapchain::ChooseExtent(uint32_t width,
                                         uint32_t height) const {
  VkSurfaceCapabilitiesKHR capabilities{};
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device_.PhysicalDevice(), surface_,
                                            &capabilities);
  // A current extent of zero (minimized window) or the sentinel maximum means
  // the surface size is not usable; fall back to clamping the requested size so
  // the swapchain is never created with a zero-sized extent.
  if (capabilities.currentExtent.width != 0 &&
      capabilities.currentExtent.height != 0 &&
      capabilities.currentExtent.width !=
          std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  }
  VkExtent2D extent{};
  extent.width =
      std::clamp(width, capabilities.minImageExtent.width,
                 capabilities.maxImageExtent.width);
  extent.height =
      std::clamp(height, capabilities.minImageExtent.height,
                 capabilities.maxImageExtent.height);
  return extent;
}

void VulkanSwapchain::CreateSwapchain(uint32_t width, uint32_t height) {
  VkSurfaceFormatKHR surface_format = ChooseSurfaceFormat();
  VkPresentModeKHR present_mode = ChoosePresentMode();
  extent_ = ChooseExtent(width, height);

  VkSurfaceCapabilitiesKHR capabilities{};
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device_.PhysicalDevice(), surface_,
                                            &capabilities);
  uint32_t image_count = capabilities.minImageCount + 1;
  if (capabilities.maxImageCount > 0 &&
      image_count > capabilities.maxImageCount) {
    image_count = capabilities.maxImageCount;
  }
  min_image_count_ = image_count;

  VkSwapchainCreateInfoKHR create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  create_info.surface = surface_;
  create_info.minImageCount = image_count;
  create_info.imageFormat = surface_format.format;
  create_info.imageColorSpace = surface_format.colorSpace;
  create_info.imageExtent = extent_;
  create_info.imageArrayLayers = 1;
  create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  uint32_t queue_family_indices[] = {device_.QueueFamilyIndex(),
                                     device_.PresentQueueFamilyIndex()};
  if (device_.QueueFamilyIndex() != device_.PresentQueueFamilyIndex()) {
    create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    create_info.queueFamilyIndexCount = 2;
    create_info.pQueueFamilyIndices = queue_family_indices;
  } else {
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }
  create_info.preTransform = capabilities.currentTransform;
  create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  create_info.presentMode = present_mode;
  create_info.clipped = VK_TRUE;
  create_info.oldSwapchain = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(device_.Device(), &create_info, nullptr,
                           &swapchain_) != VK_SUCCESS) {
    SPDLOG_ERROR("Failed to create the Vulkan swapchain.");
    throw RendererCreationException("Failed to create the Vulkan swapchain.");
  }

  image_format_ = surface_format.format;
  uint32_t swapchain_image_count = 0;
  vkGetSwapchainImagesKHR(device_.Device(), swapchain_, &swapchain_image_count,
                          nullptr);
  images_.resize(swapchain_image_count);
  vkGetSwapchainImagesKHR(device_.Device(), swapchain_, &swapchain_image_count,
                          images_.data());
}

void VulkanSwapchain::CreateImageViews() {
  image_views_.reserve(images_.size());
  for (VkImage image : images_) {
    VkImageViewCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.image = image;
    create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    create_info.format = image_format_;
    create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = 1;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = 1;

    VkImageView image_view = VK_NULL_HANDLE;
    if (vkCreateImageView(device_.Device(), &create_info, nullptr,
                          &image_view) != VK_SUCCESS) {
      SPDLOG_ERROR("Failed to create a Vulkan swapchain image view.");
      throw RendererCreationException(
          "Failed to create a Vulkan swapchain image view.");
    }
    image_views_.push_back(image_view);
  }
}

}  // namespace skeleton
