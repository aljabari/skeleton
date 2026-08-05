// Copyright 2026 aljabari

#ifndef SKELEDIT_INCLUDE_SKELEDIT_VULKAN_IMGUIBACKEND_H_
#define SKELEDIT_INCLUDE_SKELEDIT_VULKAN_IMGUIBACKEND_H_

#include <volk.h>

#include <cstdint>
#include <vector>

#include "skeledit/imguibackend.h"

struct GLFWwindow;

namespace skeleton {

class VulkanRenderer;

// Pairs the GLFW platform backend with the ImGui Vulkan renderer backend. The
// backend owns the Vulkan objects ImGui_ImplVulkan needs that the renderer does
// not provide: a descriptor pool, a render pass that loads the swapchain image
// produced by the renderer's scene render pass, a framebuffer per swapchain
// image, and a command buffer. Each frame the renderer asks the backend to
// record that command buffer through its frame submit hook, then submits it
// after its own scene command buffer and before presenting, so the UI
// composites over the scene. RenderDrawData does nothing because the drawing is
// deferred. The renderer must be fully initialised and stay alive for the
// backend's lifetime.
class VulkanImGuiBackend : public ImGuiBackend {
 public:
  VulkanImGuiBackend(GLFWwindow* window, VulkanRenderer* renderer);
  ~VulkanImGuiBackend() override;

  VulkanImGuiBackend(const VulkanImGuiBackend&) = delete;
  VulkanImGuiBackend& operator=(const VulkanImGuiBackend&) = delete;

  bool Init() override;
  void NewFrame() override;
  void RenderDrawData() override;
  ImTextureID GetViewportTextureId() const override;
  void Shutdown() override;

 private:
  // Records the ImGui draw data into the backend's command buffer, rendering
  // through its own render pass over swapchain image |image_index|, and returns
  // the command buffer for the renderer to submit. Returns VK_NULL_HANDLE when
  // the frame cannot be recorded.
  VkCommandBuffer RecordDrawData(uint32_t image_index);

  // Destroys the descriptor pool, render pass, framebuffers, and command pool
  // owned by the backend. Must run after ImGui_ImplVulkan_Shutdown so the
  // ImGui pipeline (which references the render pass) and the font atlas's
  // descriptor set are released first, and after the device is idle so no
  // recorded command buffer is still executing. The viewport texture's
  // descriptor set is freed from the pool by Shutdown before
  // ImGui_ImplVulkan_Shutdown, since that call destroys the backend data
  // ImGui_ImplVulkan_RemoveTexture depends on.
  void DestroyVulkanResources();

  // Destroys the existing framebuffers and creates one for every current
  // swapchain image view, updating extent_. The render pass is left untouched
  // because it only depends on the swapchain format. Called by the renderer's
  // swapchain-recreated callback after the device is idle. Returns false and
  // logs if a framebuffer cannot be created, leaving framebuffers_ empty.
  bool RebuildFramebuffers();

  GLFWwindow* window_;
  VulkanRenderer* renderer_;
  VkDescriptorPool descriptor_pool_ = VK_NULL_HANDLE;
  VkRenderPass render_pass_ = VK_NULL_HANDLE;
  VkCommandPool command_pool_ = VK_NULL_HANDLE;
  VkCommandBuffer command_buffer_ = VK_NULL_HANDLE;
  std::vector<VkFramebuffer> framebuffers_;
  VkExtent2D extent_{};
  // The render target image view registered with ImGui as the viewport texture
  // and the descriptor set ImGui returned for it (the ImTextureID the editor
  // draws). The descriptor set is re-registered when the renderer recreates its
  // render target, for example on resize. They are mutated lazily from the const
  // GetViewportTextureId, hence mutable.
  mutable VkImageView viewport_image_view_ = VK_NULL_HANDLE;
  mutable VkDescriptorSet viewport_descriptor_set_ = VK_NULL_HANDLE;
  bool initialised_ = false;
};

}  // namespace skeleton

#endif  // SKELEDIT_INCLUDE_SKELEDIT_VULKAN_IMGUIBACKEND_H_
