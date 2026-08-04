// Copyright 2026 aljabari

#ifndef LIBSKELETON_VULKAN_VULKANRENDERER_H_
#define LIBSKELETON_VULKAN_VULKANRENDERER_H_

#include <volk.h>

#include <functional>
#include <memory>
#include <vector>

#include "libskeleton/renderer.h"

struct GLFWwindow;

namespace skeleton {

class VulkanCommandBuffer;
class VulkanDevice;
class VulkanFence;
class VulkanFramebuffer;
class VulkanGraphicsPipeline;
class VulkanInstance;
class VulkanMesh;
class VulkanRenderPass;
class VulkanSemaphore;
class VulkanSwapchain;

class VulkanRenderer : public Renderer {
 public:
  VulkanRenderer();
  ~VulkanRenderer() override;

  RendererBackend GetBackend() const override;
  void CreateContext(const WindowConfig& config) override;
  GLFWwindow* GetNativeWindow() const override;
  void Render() override;

  // CreateContext creates the window and instance, then the surface, physical
  // device, logical device, and queues, and finally the swapchain, render
  // pass, graphics pipeline, triangle mesh, framebuffers, command buffer, and
  // synchronisation objects used by Render. Handles are VK_NULL_HANDLE until
  // their creation step succeeds, and every step throws
  // RendererCreationException on failure. The window is owned by the renderer
  // and destroyed with it.
  VkInstance Instance() const;
  VkSurfaceKHR Surface() const;
  VkPhysicalDevice PhysicalDevice() const;
  VkDevice Device() const;
  VkQueue GraphicsQueue() const;
  VkQueue PresentQueue() const;
  VkRenderPass RenderPass() const;
  uint32_t QueueFamilyIndex() const;
  uint32_t SwapchainImageCount() const;
  uint32_t SwapchainMinImageCount() const;

  // Invoked on the command buffer while it is recording, after the scene is
  // drawn and before the render pass ends. Callers use it to composite their
  // own draw calls (for example a UI overlay) over the frame. The callback is
  // called only for frames whose command buffer is actually recorded; frames
  // skipped because the swapchain is out of date do not invoke it. Pass an
  // empty callback to remove it.
  using OverlayDrawCallback = std::function<void(VkCommandBuffer)>;
  void SetOverlayDrawCallback(OverlayDrawCallback callback);

 private:
  void CreateSwapchainResources(const WindowConfig& config);

  GLFWwindow* window_ = nullptr;
  std::unique_ptr<VulkanInstance> instance_;
  std::unique_ptr<VulkanDevice> device_;
  std::unique_ptr<VulkanSwapchain> swapchain_;
  std::unique_ptr<VulkanRenderPass> render_pass_;
  std::unique_ptr<VulkanGraphicsPipeline> graphics_pipeline_;
  std::unique_ptr<VulkanMesh> mesh_;
  std::vector<std::unique_ptr<VulkanFramebuffer>> framebuffers_;
  std::unique_ptr<VulkanCommandBuffer> command_buffer_;
  std::unique_ptr<VulkanSemaphore> image_available_semaphore_;
  std::unique_ptr<VulkanSemaphore> render_finished_semaphore_;
  std::unique_ptr<VulkanFence> in_flight_fence_;
  VkSurfaceKHR surface_ = VK_NULL_HANDLE;
  OverlayDrawCallback overlay_draw_callback_;
};

}  // namespace skeleton

#endif  // LIBSKELETON_VULKAN_VULKANRENDERER_H_
