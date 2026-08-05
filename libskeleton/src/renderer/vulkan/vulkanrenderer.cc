// Copyright 2026 aljabari

#include "libskeleton/vulkan/vulkanrenderer.h"

#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "libskeleton/renderer.h"
#include "renderer/vulkan/vulkancommandbuffer.h"
#include "renderer/vulkan/vulkandevice.h"
#include "renderer/vulkan/vulkanfence.h"
#include "renderer/vulkan/vulkanframebuffer.h"
#include "renderer/vulkan/vulkangraphicspipeline.h"
#include "renderer/vulkan/vulkaninstance.h"
#include "renderer/vulkan/vulkanmesh.h"
#include "renderer/vulkan/vulkanrenderpass.h"
#include "renderer/vulkan/vulkansemaphore.h"
#include "renderer/vulkan/vulkanswapchain.h"

namespace skeleton {

namespace {

// The same hardcoded triangle mesh the OpenGL renderer draws, authored in the
// Vulkan coordinate system (front faces wind counter-clockwise in the
// y-down framebuffer): three vertices of interleaved position (vec3) and
// colour (vec3).
const std::vector<float> kTriangleVertices = {
    -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  //
    0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f,  //
    0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  //
};

// The background the triangle is cleared to before each frame, matching the
// OpenGL renderer.
const VkClearColorValue kClearColor = {{0.2f, 0.3f, 0.8f, 1.0f}};

}  // namespace

VulkanRenderer::VulkanRenderer() = default;

VulkanRenderer::~VulkanRenderer() {
  // The swapchain references the surface, so it must be destroyed before the
  // surface. The remaining frame resources only reference the device and are
  // destroyed as members below.
  swapchain_.reset();
  if (instance_ != nullptr && surface_ != VK_NULL_HANDLE) {
    vkDestroySurfaceKHR(instance_->Instance(), surface_, nullptr);
  }
  if (window_ != nullptr) {
    glfwDestroyWindow(window_);
  }
}

RendererBackend VulkanRenderer::GetBackend() const {
  return RendererBackend::kVulkan;
}

void VulkanRenderer::CreateContext(const WindowConfig& config) {
  InitGlfw();
  uint32_t extension_count = 0;
  const char** required_extensions =
      glfwGetRequiredInstanceExtensions(&extension_count);
  std::vector<const char*> instance_extensions;
  if (required_extensions != nullptr) {
    instance_extensions.assign(required_extensions,
                               required_extensions + extension_count);
  }
  instance_ = std::make_unique<VulkanInstance>(instance_extensions);

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  window_ = CreateGlfwWindow(config);

  if (glfwCreateWindowSurface(instance_->Instance(), window_, nullptr,
                              &surface_) != VK_SUCCESS) {
    SPDLOG_ERROR("Failed to create the Vulkan surface.");
    throw RendererCreationException("Failed to create the Vulkan surface.");
  }
  SPDLOG_INFO("Created Vulkan surface.");
  device_ = std::make_unique<VulkanDevice>(*instance_, surface_);
  SPDLOG_INFO("Created Vulkan context for window \"{}\" ({}x{}).",
              config.title, config.width, config.height);
  CreateSwapchainResources(config);
}

void VulkanRenderer::CreateSwapchainResources(const WindowConfig& config) {
  swapchain_ = std::make_unique<VulkanSwapchain>(
      *device_, surface_, static_cast<uint32_t>(config.width),
      static_cast<uint32_t>(config.height));
  render_pass_ =
      std::make_unique<VulkanRenderPass>(*device_, swapchain_->ImageFormat());

  const std::string shader_directory = SKELETON_SHADER_DIR;
  graphics_pipeline_ = std::make_unique<VulkanGraphicsPipeline>(
      *device_, render_pass_->RenderPass(), swapchain_->Extent(),
      shader_directory + "/triangle.vert.spv",
      shader_directory + "/triangle.frag.spv");
  mesh_ = std::make_unique<VulkanMesh>(*device_, kTriangleVertices);

  framebuffers_.reserve(swapchain_->ImageViews().size());
  for (VkImageView image_view : swapchain_->ImageViews()) {
    framebuffers_.push_back(std::make_unique<VulkanFramebuffer>(
        *device_, render_pass_->RenderPass(), image_view,
        swapchain_->Extent()));
  }

  command_buffer_ = std::make_unique<VulkanCommandBuffer>(*device_);
  image_available_semaphore_ = std::make_unique<VulkanSemaphore>(*device_);
  render_finished_semaphore_ = std::make_unique<VulkanSemaphore>(*device_);
  in_flight_fence_ = std::make_unique<VulkanFence>(*device_);
}

GLFWwindow* VulkanRenderer::GetNativeWindow() const {
  return window_;
}

void VulkanRenderer::Render() {
  if (device_ == nullptr || swapchain_ == nullptr ||
      graphics_pipeline_ == nullptr || mesh_ == nullptr) {
    SPDLOG_WARN("Vulkan renderer is not fully initialised; skipping frame.");
    return;
  }

  const VkDevice device = device_->Device();
  VkFence in_flight_fence = in_flight_fence_->Fence();
  vkWaitForFences(device, 1, &in_flight_fence, VK_TRUE, UINT64_MAX);

  uint32_t image_index = 0;
  const VkResult acquire_result = vkAcquireNextImageKHR(
      device, swapchain_->Swapchain(), UINT64_MAX,
      image_available_semaphore_->Semaphore(), VK_NULL_HANDLE, &image_index);
  if (acquire_result == VK_ERROR_OUT_OF_DATE_KHR) {
    // The swapchain no longer matches the window (for example it was
    // resized); recreate it to match and skip this frame. No submit was made,
    // so the fence is left signaled and the next frame waits normally.
    SPDLOG_DEBUG("Swapchain out of date; recreating and skipping frame.");
    RecreateSwapchain();
    return;
  }
  if (acquire_result != VK_SUCCESS && acquire_result != VK_SUBOPTIMAL_KHR) {
    SPDLOG_ERROR("Failed to acquire a swapchain image.");
    throw RendererCreationException("Failed to acquire a swapchain image.");
  }
  // Only reset the fence once this frame will actually submit work.
  vkResetFences(device, 1, &in_flight_fence);

  VkCommandBuffer command_buffer = command_buffer_->CommandBuffer();
  VkCommandBufferBeginInfo begin_info{};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  vkBeginCommandBuffer(command_buffer, &begin_info);

  VkRenderPassBeginInfo render_pass_info{};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_info.renderPass = render_pass_->RenderPass();
  render_pass_info.framebuffer = framebuffers_[image_index]->Framebuffer();
  render_pass_info.renderArea.offset = {0, 0};
  render_pass_info.renderArea.extent = swapchain_->Extent();
  VkClearValue clear_value{};
  clear_value.color = kClearColor;
  render_pass_info.clearValueCount = 1;
  render_pass_info.pClearValues = &clear_value;
  vkCmdBeginRenderPass(command_buffer, &render_pass_info,
                       VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    graphics_pipeline_->Pipeline());
  VkBuffer vertex_buffer = mesh_->Buffer();
  VkDeviceSize offset = 0;
  vkCmdBindVertexBuffers(command_buffer, 0, 1, &vertex_buffer, &offset);
  vkCmdDraw(command_buffer, static_cast<uint32_t>(mesh_->VertexCount()), 1, 0,
            0);
  vkCmdEndRenderPass(command_buffer);

  if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
    SPDLOG_ERROR("Failed to record the Vulkan draw commands.");
    throw RendererCreationException(
        "Failed to record the Vulkan draw commands.");
  }

  // The additional command buffer renders after the scene's, so it composites
  // over the same swapchain image through its own render pass. Both command
  // buffers are submitted together and execute in order on the graphics queue.
  std::vector<VkCommandBuffer> command_buffers;
  command_buffers.push_back(command_buffer);
  if (frame_submit_callback_ != nullptr) {
    VkCommandBuffer additional_command_buffer =
        frame_submit_callback_(image_index);
    if (additional_command_buffer != VK_NULL_HANDLE) {
      command_buffers.push_back(additional_command_buffer);
    }
  }

  VkPipelineStageFlags wait_stage =
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  VkSemaphore wait_semaphore = image_available_semaphore_->Semaphore();
  VkSemaphore signal_semaphore = render_finished_semaphore_->Semaphore();
  VkSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = &wait_semaphore;
  submit_info.pWaitDstStageMask = &wait_stage;
  submit_info.commandBufferCount =
      static_cast<uint32_t>(command_buffers.size());
  submit_info.pCommandBuffers = command_buffers.data();
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = &signal_semaphore;
  if (vkQueueSubmit(device_->GraphicsQueue(), 1, &submit_info,
                    in_flight_fence) != VK_SUCCESS) {
    SPDLOG_ERROR("Failed to submit the Vulkan draw commands.");
    throw RendererCreationException(
        "Failed to submit the Vulkan draw commands.");
  }

  VkSwapchainKHR swapchain = swapchain_->Swapchain();
  VkPresentInfoKHR present_info{};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = &signal_semaphore;
  present_info.swapchainCount = 1;
  present_info.pSwapchains = &swapchain;
  present_info.pImageIndices = &image_index;
  const VkResult present_result =
      vkQueuePresentKHR(device_->PresentQueue(), &present_info);
  if (present_result == VK_ERROR_OUT_OF_DATE_KHR ||
      present_result == VK_SUBOPTIMAL_KHR) {
    // The swapchain no longer matches the window; the frame was still
    // submitted and presented, so recreate the swapchain to match and let a
    // subsequent frame continue normally.
    SPDLOG_DEBUG("Swapchain present out of date or suboptimal; recreating.");
    RecreateSwapchain();
  } else if (present_result != VK_SUCCESS) {
    SPDLOG_ERROR("Failed to present the Vulkan swapchain image.");
    throw RendererCreationException(
        "Failed to present the Vulkan swapchain image.");
  }
}

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

VkRenderPass VulkanRenderer::RenderPass() const {
  return render_pass_ ? render_pass_->RenderPass() : VK_NULL_HANDLE;
}

uint32_t VulkanRenderer::QueueFamilyIndex() const {
  return device_ ? device_->QueueFamilyIndex() : 0;
}

uint32_t VulkanRenderer::SwapchainImageCount() const {
  return swapchain_ ? static_cast<uint32_t>(swapchain_->ImageViews().size())
                    : 0;
}

uint32_t VulkanRenderer::SwapchainMinImageCount() const {
  return swapchain_ ? swapchain_->MinImageCount() : 0;
}

VkFormat VulkanRenderer::SwapchainImageFormat() const {
  return swapchain_ ? swapchain_->ImageFormat() : VK_FORMAT_UNDEFINED;
}

VkExtent2D VulkanRenderer::SwapchainExtent() const {
  return swapchain_ ? swapchain_->Extent() : VkExtent2D{};
}

const std::vector<VkImageView>& VulkanRenderer::SwapchainImageViews() const {
  static const std::vector<VkImageView> kEmpty;
  return swapchain_ ? swapchain_->ImageViews() : kEmpty;
}

void VulkanRenderer::RecreateSwapchain() {
  if (device_ == nullptr || window_ == nullptr || render_pass_ == nullptr) {
    return;
  }
  // Wait so no in-flight command buffer (including any recorded by the frame
  // submit hook, which uses the same swapchain images) still references the
  // framebuffers about to be destroyed.
  vkDeviceWaitIdle(device_->Device());

  int width = 0;
  int height = 0;
  glfwGetFramebufferSize(window_, &width, &height);
  const uint32_t swapchain_width = std::max(width, 1);
  const uint32_t swapchain_height = std::max(height, 1);

  // The image views are owned by the swapchain, so the framebuffers built on
  // top of them must be destroyed before the swapchain is.
  framebuffers_.clear();
  swapchain_.reset();
  swapchain_ = std::make_unique<VulkanSwapchain>(
      *device_, surface_, swapchain_width, swapchain_height);

  framebuffers_.reserve(swapchain_->ImageViews().size());
  for (VkImageView image_view : swapchain_->ImageViews()) {
    framebuffers_.push_back(std::make_unique<VulkanFramebuffer>(
        *device_, render_pass_->RenderPass(), image_view,
        swapchain_->Extent()));
  }

  if (swapchain_recreated_callback_ != nullptr) {
    swapchain_recreated_callback_();
  }
}

void VulkanRenderer::SetFrameSubmitCallback(FrameSubmitCallback callback) {
  frame_submit_callback_ = std::move(callback);
}

void VulkanRenderer::SetSwapchainRecreatedCallback(
    SwapchainRecreatedCallback callback) {
  swapchain_recreated_callback_ = std::move(callback);
}

}  // namespace skeleton
