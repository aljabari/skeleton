// Copyright 2026 aljabari

#include "libskeleton/vulkan/vulkanrenderer.h"

#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

#include <memory>
#include <vector>

#include "libskeleton/renderer.h"
#include "renderer/vulkan/vulkandevice.h"
#include "renderer/vulkan/vulkaninstance.h"

namespace skeleton {

VulkanRenderer::VulkanRenderer() {
  if (glfwInit() != GLFW_TRUE) {
    SPDLOG_ERROR("Failed to initialise GLFW.");
    throw RendererCreationException("Failed to initialise GLFW.");
  }
  uint32_t extension_count = 0;
  const char** required_extensions =
      glfwGetRequiredInstanceExtensions(&extension_count);
  std::vector<const char*> instance_extensions;
  if (required_extensions != nullptr) {
    instance_extensions.assign(required_extensions,
                               required_extensions + extension_count);
  }
  instance_ = std::make_unique<VulkanInstance>(instance_extensions);
}

VulkanRenderer::~VulkanRenderer() {
  if (instance_ != nullptr && surface_ != VK_NULL_HANDLE) {
    vkDestroySurfaceKHR(instance_->Instance(), surface_, nullptr);
  }
}

RendererBackend VulkanRenderer::GetBackend() const {
  return RendererBackend::kVulkan;
}

void VulkanRenderer::SetWindowHints() {
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

void VulkanRenderer::InitialiseForWindow(GLFWwindow* window) {
  if (glfwCreateWindowSurface(instance_->Instance(), window, nullptr,
                              &surface_) != VK_SUCCESS) {
    SPDLOG_ERROR("Failed to create the Vulkan surface.");
    throw RendererCreationException("Failed to create the Vulkan surface.");
  }
  SPDLOG_INFO("Created Vulkan surface.");
  device_ = std::make_unique<VulkanDevice>(*instance_, surface_);
}

void VulkanRenderer::Render() {}

VkInstance VulkanRenderer::Instance() const {
  return instance_ ? instance_->Instance() : VK_NULL_HANDLE;
}

VkSurfaceKHR VulkanRenderer::Surface() const {
  return surface_;
}

VkPhysicalDevice VulkanRenderer::PhysicalDevice() const {
  return device_ ? device_->PhysicalDevice() : VK_NULL_HANDLE;
}

VkDevice VulkanRenderer::Device() const {
  return device_ ? device_->Device() : VK_NULL_HANDLE;
}

VkQueue VulkanRenderer::GraphicsQueue() const {
  return device_ ? device_->GraphicsQueue() : VK_NULL_HANDLE;
}

VkQueue VulkanRenderer::PresentQueue() const {
  return device_ ? device_->PresentQueue() : VK_NULL_HANDLE;
}

}  // namespace skeleton
