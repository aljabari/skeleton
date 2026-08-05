// Copyright 2026 aljabari

#include "renderer/vulkan/vulkanrendertarget.h"

#include <spdlog/spdlog.h>

#include <cstdint>

#include "libskeleton/renderer.h"

namespace skeleton {

VulkanRenderTarget::VulkanRenderTarget(const VulkanDevice& device,
                                       VkFormat format, uint32_t width,
                                       uint32_t height)
    : device_(device), format_(format), extent_{width, height} {
  try {
    CreateImage();
    CreateImageView();
  } catch (...) {
    if (image_view_ != VK_NULL_HANDLE) {
      vkDestroyImageView(device_.Device(), image_view_, nullptr);
    }
    if (memory_ != VK_NULL_HANDLE) {
      vkFreeMemory(device_.Device(), memory_, nullptr);
    }
    if (image_ != VK_NULL_HANDLE) {
      vkDestroyImage(device_.Device(), image_, nullptr);
    }
    throw;
  }
  SPDLOG_DEBUG("Created Vulkan render target ({}x{}).", extent_.width,
               extent_.height);
}

VulkanRenderTarget::~VulkanRenderTarget() {
  if (image_view_ != VK_NULL_HANDLE) {
    vkDestroyImageView(device_.Device(), image_view_, nullptr);
  }
  if (memory_ != VK_NULL_HANDLE) {
    vkFreeMemory(device_.Device(), memory_, nullptr);
  }
  if (image_ != VK_NULL_HANDLE) {
    vkDestroyImage(device_.Device(), image_, nullptr);
  }
}

VkImage VulkanRenderTarget::Image() const {
  return image_;
}

VkImageView VulkanRenderTarget::ImageView() const {
  return image_view_;
}

VkExtent2D VulkanRenderTarget::Extent() const {
  return extent_;
}

VkFormat VulkanRenderTarget::Format() const {
  return format_;
}

uint32_t VulkanRenderTarget::FindMemoryType(
    uint32_t type_bits, VkMemoryPropertyFlags properties) const {
  VkPhysicalDeviceMemoryProperties memory_properties{};
  vkGetPhysicalDeviceMemoryProperties(device_.PhysicalDevice(),
                                      &memory_properties);
  for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i) {
    if ((type_bits & (1u << i)) != 0 &&
        (memory_properties.memoryTypes[i].propertyFlags & properties) ==
            properties) {
      return i;
    }
  }
  SPDLOG_ERROR("Failed to find a suitable Vulkan memory type.");
  throw RendererCreationException(
      "Failed to find a suitable Vulkan memory type.");
}

void VulkanRenderTarget::CreateImage() {
  VkImageCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  create_info.imageType = VK_IMAGE_TYPE_2D;
  create_info.format = format_;
  create_info.extent = {extent_.width, extent_.height, 1};
  create_info.mipLevels = 1;
  create_info.arrayLayers = 1;
  create_info.samples = VK_SAMPLE_COUNT_1_BIT;
  create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
  create_info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                      VK_IMAGE_USAGE_SAMPLED_BIT;
  create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  if (vkCreateImage(device_.Device(), &create_info, nullptr, &image_) !=
      VK_SUCCESS) {
    SPDLOG_ERROR("Failed to create the Vulkan render target image.");
    throw RendererCreationException(
        "Failed to create the Vulkan render target image.");
  }

  VkMemoryRequirements memory_requirements{};
  vkGetImageMemoryRequirements(device_.Device(), image_, &memory_requirements);
  VkMemoryAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.allocationSize = memory_requirements.size;
  alloc_info.memoryTypeIndex =
      FindMemoryType(memory_requirements.memoryTypeBits,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  if (vkAllocateMemory(device_.Device(), &alloc_info, nullptr, &memory_) !=
      VK_SUCCESS) {
    SPDLOG_ERROR("Failed to allocate memory for the Vulkan render target.");
    throw RendererCreationException(
        "Failed to allocate memory for the Vulkan render target.");
  }
  if (vkBindImageMemory(device_.Device(), image_, memory_, 0) != VK_SUCCESS) {
    SPDLOG_ERROR("Failed to bind memory to the Vulkan render target image.");
    throw RendererCreationException(
        "Failed to bind memory to the Vulkan render target image.");
  }
}

void VulkanRenderTarget::CreateImageView() {
  VkImageViewCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  create_info.image = image_;
  create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  create_info.format = format_;
  create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
  create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  create_info.subresourceRange.baseMipLevel = 0;
  create_info.subresourceRange.levelCount = 1;
  create_info.subresourceRange.baseArrayLayer = 0;
  create_info.subresourceRange.layerCount = 1;
  if (vkCreateImageView(device_.Device(), &create_info, nullptr,
                        &image_view_) != VK_SUCCESS) {
    SPDLOG_ERROR("Failed to create the Vulkan render target image view.");
    throw RendererCreationException(
        "Failed to create the Vulkan render target image view.");
  }
}

}  // namespace skeleton
