// Copyright 2026 aljabari

#include "libskeleton/vulkan/vulkanrenderer.h"

#include <GLFW/glfw3.h>

namespace skeleton {

VulkanRenderer::VulkanRenderer() = default;

VulkanRenderer::~VulkanRenderer() = default;

void VulkanRenderer::SetWindowHints() {
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

void VulkanRenderer::Render() {}

}  // namespace skeleton
