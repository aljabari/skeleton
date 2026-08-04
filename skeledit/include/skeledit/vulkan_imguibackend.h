// Copyright 2026 aljabari

#ifndef SKELEDIT_VULKAN_IMGUIBACKEND_H_
#define SKELEDIT_VULKAN_IMGUIBACKEND_H_

#include "skeledit/imguibackend.h"

struct GLFWwindow;

namespace skeleton {

class VulkanRenderer;

// Pairs the GLFW platform backend with the ImGui Vulkan renderer backend. The
// ImGui draw data is composited over the swapchain image by the renderer's
// overlay hook inside its render pass, so RenderDrawData does nothing here.
// The renderer must be fully initialised and stay alive for the backend's
// lifetime.
class VulkanImGuiBackend : public ImGuiBackend {
 public:
  VulkanImGuiBackend(GLFWwindow* window, VulkanRenderer* renderer);
  ~VulkanImGuiBackend() override;

  VulkanImGuiBackend(const VulkanImGuiBackend&) = delete;
  VulkanImGuiBackend& operator=(const VulkanImGuiBackend&) = delete;

  bool Init() override;
  void NewFrame() override;
  void RenderDrawData() override;
  void Shutdown() override;

 private:
  GLFWwindow* window_;
  VulkanRenderer* renderer_;
  bool initialised_ = false;
};

}  // namespace skeleton

#endif  // SKELEDIT_VULKAN_IMGUIBACKEND_H_
