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

  // The swapchain the renderer presents to. The image views are owned by the
  // renderer and stay valid until the swapchain is recreated (or the renderer
  // is destroyed), so callers (for example the editor's ImGui backend) can use
  // them to build framebuffers for their own render pass over the same
  // swapchain images.
  VkFormat SwapchainImageFormat() const;
  VkExtent2D SwapchainExtent() const;
  const std::vector<VkImageView>& SwapchainImageViews() const;

  // Invoked once per recorded frame, after the swapchain image has been
  // acquired and the renderer's own command buffer has been recorded. The
  // callback records and ends an additional command buffer (for example an
  // ImGui UI drawn through its own render pass over the same swapchain image)
  // and returns it; the renderer submits it after its own command buffer and
  // before presenting. |image_index| is the index of the acquired swapchain
  // image, which the callback uses to pick the matching framebuffer. Return
  // VK_NULL_HANDLE to submit nothing extra. Frames skipped because the
  // swapchain is out of date do not invoke the callback. Pass an empty
  // callback to remove it.
  using FrameSubmitCallback =
      std::function<VkCommandBuffer(uint32_t image_index)>;
  void SetFrameSubmitCallback(FrameSubmitCallback callback);

  // Invoked after the renderer recreates its swapchain and framebuffers (when
  // the swapchain becomes out of date, for example because the window was
  // resized). The device is idle by then. Callers holding framebuffers that
  // reference the swapchain image views (for example the editor's ImGui
  // backend) must destroy them here and rebuild them from the new
  // SwapchainImageViews() and SwapchainExtent(). Pass an empty callback to
  // remove it.
  using SwapchainRecreatedCallback = std::function<void()>;
  void SetSwapchainRecreatedCallback(SwapchainRecreatedCallback callback);

 private:
  void CreateSwapchainResources(const WindowConfig& config);
  // Destroys and recreates the swapchain and its framebuffers to match the
  // current window size. Called when the swapchain is out of date; waits for
  // the device to go idle first so no in-flight command buffer references the
  // destroyed framebuffers.
  void RecreateSwapchain();

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
  FrameSubmitCallback frame_submit_callback_;
  SwapchainRecreatedCallback swapchain_recreated_callback_;
};

}  // namespace skeleton

#endif  // LIBSKELETON_VULKAN_VULKANRENDERER_H_
