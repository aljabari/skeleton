// Copyright 2026 aljabari

#include "libskeleton/vulkan/vulkanrenderer.h"

#include <GLFW/glfw3.h>

namespace skeleton {

VulkanRenderer::VulkanRenderer() {
  throw RendererCreationException(
      "The Vulkan renderer is not implemented yet.");
}

VulkanRenderer::~VulkanRenderer() = default;

RendererBackend VulkanRenderer::GetBackend() const {
  return RendererBackend::kVulkan;
}

void VulkanRenderer::SetWindowHints() {
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

void VulkanRenderer::InitialiseForWindow(GLFWwindow* window) {}

void VulkanRenderer::Render() {}

}  // namespace skeleton
