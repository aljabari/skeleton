// Copyright 2026 aljabari

#include "renderer/vulkan/vulkansemaphore.h"

#include <spdlog/spdlog.h>

#include "libskeleton/renderer.h"

namespace skeleton {

VulkanSemaphore::VulkanSemaphore(const VulkanDevice& device) : device_(device) {
  VkSemaphoreCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  if (vkCreateSemaphore(device_.Device(), &create_info, nullptr, &semaphore_) !=
      VK_SUCCESS) {
    SPDLOG_ERROR("Failed to create the Vulkan semaphore.");
    throw RendererCreationException("Failed to create the Vulkan semaphore.");
  }
  SPDLOG_DEBUG("Created Vulkan semaphore.");
}

VulkanSemaphore::~VulkanSemaphore() {
  if (semaphore_ != VK_NULL_HANDLE) {
    vkDestroySemaphore(device_.Device(), semaphore_, nullptr);
  }
}

VkSemaphore VulkanSemaphore::Semaphore() const {
  return semaphore_;
}

}  // namespace skeleton
