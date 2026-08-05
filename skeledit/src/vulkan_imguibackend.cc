// Copyright 2026 aljabari

#include "skeledit/vulkan_imguibackend.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>
#include <spdlog/spdlog.h>
#include <volk.h>

#include <cstdint>

#include "libskeleton/vulkan/vulkanrenderer.h"

namespace skeleton {

namespace {

// The ImGui Vulkan backend allocates descriptor sets from the backend's
// descriptor pool: at least
// IMGUI_IMPL_VULKAN_MINIMUM_SAMPLED_IMAGE_POOL_SIZE descriptors for the font
// atlas, plus headroom for textures the editor adds later (for example the
// render target preview).
constexpr uint32_t kDescriptorPoolExtraSets = 8;

void LogVulkanResult(VkResult result) {
  if (result != VK_SUCCESS) {
    SPDLOG_ERROR("Vulkan call failed inside the ImGui backend ({}).",
                 static_cast<int>(result));
  }
}

bool CreateDescriptorPool(VkDevice device, VkDescriptorPool* pool) {
  const uint32_t sampled_image_count =
      IMGUI_IMPL_VULKAN_MINIMUM_SAMPLED_IMAGE_POOL_SIZE +
      kDescriptorPoolExtraSets;
  const uint32_t sampler_count = IMGUI_IMPL_VULKAN_MINIMUM_SAMPLER_POOL_SIZE;
  VkDescriptorPoolSize pool_sizes[] = {
      {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, sampled_image_count},
      {VK_DESCRIPTOR_TYPE_SAMPLER, sampler_count},
  };
  VkDescriptorPoolCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  create_info.maxSets = sampled_image_count + sampler_count;
  create_info.poolSizeCount = 2;
  create_info.pPoolSizes = pool_sizes;
  if (vkCreateDescriptorPool(device, &create_info, nullptr, pool) !=
      VK_SUCCESS) {
    SPDLOG_ERROR("Failed to create the ImGui descriptor pool.");
    return false;
  }
  return true;
}

}  // namespace

VulkanImGuiBackend::VulkanImGuiBackend(GLFWwindow* window,
                                       VulkanRenderer* renderer)
    : window_(window), renderer_(renderer) {}

VulkanImGuiBackend::~VulkanImGuiBackend() {
  if (initialised_) {
    Shutdown();
  }
}

bool VulkanImGuiBackend::Init() {
  if (renderer_ == nullptr || !ImGui_ImplGlfw_InitForVulkan(window_, true)) {
    return false;
  }

  const VkDevice device = renderer_->Device();
  if (!CreateDescriptorPool(device, &descriptor_pool_)) {
    ImGui_ImplGlfw_Shutdown();
    return false;
  }

  // The render pass loads the swapchain image left behind by the renderer's
  // scene render pass (which ends in the presentation-ready layout) and leaves
  // it presentation-ready again, so the UI composites over the scene.
  VkAttachmentDescription attachment{};
  attachment.format = renderer_->SwapchainImageFormat();
  attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
  attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  attachment.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference color_reference{};
  color_reference.attachment = 0;
  color_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &color_reference;

  VkRenderPassCreateInfo render_pass_info{};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_info.attachmentCount = 1;
  render_pass_info.pAttachments = &attachment;
  render_pass_info.subpassCount = 1;
  render_pass_info.pSubpasses = &subpass;

  if (vkCreateRenderPass(device, &render_pass_info, nullptr, &render_pass_) !=
      VK_SUCCESS) {
    SPDLOG_ERROR("Failed to create the ImGui render pass.");
    DestroyVulkanResources();
    ImGui_ImplGlfw_Shutdown();
    return false;
  }

  extent_ = renderer_->SwapchainExtent();
  if (!RebuildFramebuffers()) {
    DestroyVulkanResources();
    ImGui_ImplGlfw_Shutdown();
    return false;
  }

  VkCommandPoolCreateInfo command_pool_info{};
  command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  command_pool_info.queueFamilyIndex = renderer_->QueueFamilyIndex();
  if (vkCreateCommandPool(device, &command_pool_info, nullptr,
                          &command_pool_) != VK_SUCCESS) {
    SPDLOG_ERROR("Failed to create the ImGui command pool.");
    DestroyVulkanResources();
    ImGui_ImplGlfw_Shutdown();
    return false;
  }

  VkCommandBufferAllocateInfo allocate_info{};
  allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocate_info.commandPool = command_pool_;
  allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocate_info.commandBufferCount = 1;
  if (vkAllocateCommandBuffers(device, &allocate_info, &command_buffer_) !=
      VK_SUCCESS) {
    SPDLOG_ERROR("Failed to allocate the ImGui command buffer.");
    DestroyVulkanResources();
    ImGui_ImplGlfw_Shutdown();
    return false;
  }

  ImGui_ImplVulkan_InitInfo init_info{};
  init_info.ApiVersion = VK_API_VERSION_1_0;
  init_info.Instance = renderer_->Instance();
  init_info.PhysicalDevice = renderer_->PhysicalDevice();
  init_info.Device = device;
  init_info.QueueFamily = renderer_->QueueFamilyIndex();
  init_info.Queue = renderer_->GraphicsQueue();
  init_info.DescriptorPool = descriptor_pool_;
  init_info.MinImageCount = renderer_->SwapchainMinImageCount();
  init_info.ImageCount = renderer_->SwapchainImageCount();
  init_info.PipelineInfoMain.RenderPass = render_pass_;
  init_info.PipelineInfoMain.Subpass = 0;
  init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  init_info.CheckVkResultFn = &LogVulkanResult;
  if (!ImGui_ImplVulkan_Init(&init_info)) {
    ImGui_ImplGlfw_Shutdown();
    DestroyVulkanResources();
    return false;
  }

  // The ImGui draw data is recorded into the backend's own command buffer by
  // the renderer's frame submit hook, which submits it after the scene command
  // buffer and before presenting. When the renderer recreates its swapchain
  // (for example because the window was resized), the backend rebuilds its
  // framebuffers to match the new swapchain images.
  renderer_->SetFrameSubmitCallback(
      [this](uint32_t image_index) { return RecordDrawData(image_index); });
  renderer_->SetSwapchainRecreatedCallback(
      [this]() { RebuildFramebuffers(); });
  initialised_ = true;
  return true;
}

void VulkanImGuiBackend::NewFrame() {
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
}

void VulkanImGuiBackend::RenderDrawData() {
  // Drawing happens through the renderer's frame submit hook inside
  // VulkanRenderer::Render, so there is nothing to do here.
}

ImTextureID VulkanImGuiBackend::GetViewportTextureId() const {
  const VkImageView image_view = renderer_ != nullptr
                                     ? renderer_->RenderTargetImageView()
                                     : VK_NULL_HANDLE;
  if (image_view != viewport_image_view_) {
    // The renderer recreated its render target (or started or stopped rendering
    // to one), so the descriptor set ImGui uses to sample it must be replaced.
    // The renderer waits for the device to go idle before destroying the old
    // render target, so the old descriptor set is no longer referenced by any
    // in-flight frame here.
    if (viewport_descriptor_set_ != VK_NULL_HANDLE) {
      ImGui_ImplVulkan_RemoveTexture(viewport_descriptor_set_);
      viewport_descriptor_set_ = VK_NULL_HANDLE;
    }
    viewport_image_view_ = image_view;
    if (image_view != VK_NULL_HANDLE) {
      viewport_descriptor_set_ = ImGui_ImplVulkan_AddTexture(
          image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
  }
  return reinterpret_cast<ImTextureID>(viewport_descriptor_set_);
}

void VulkanImGuiBackend::Shutdown() {
  if (renderer_ == nullptr) {
    return;
  }
  // Drop the frame submit hook and the swapchain-recreated hook before the
  // ImGui backends release their resources so a frame cannot record against a
  // half-destroyed backend.
  renderer_->SetFrameSubmitCallback({});
  renderer_->SetSwapchainRecreatedCallback({});
  // Wait for the last submitted frame so the backend's command buffer and
  // framebuffers are no longer in use before they are destroyed.
  vkDeviceWaitIdle(renderer_->Device());
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  DestroyVulkanResources();
  initialised_ = false;
}

VkCommandBuffer VulkanImGuiBackend::RecordDrawData(uint32_t image_index) {
  if (command_buffer_ == VK_NULL_HANDLE ||
      image_index >= framebuffers_.size()) {
    return VK_NULL_HANDLE;
  }

  const VkDevice device = renderer_->Device();
  vkResetCommandPool(device, command_pool_, 0);

  VkCommandBufferBeginInfo begin_info{};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  if (vkBeginCommandBuffer(command_buffer_, &begin_info) != VK_SUCCESS) {
    SPDLOG_ERROR("Failed to begin the ImGui command buffer.");
    return VK_NULL_HANDLE;
  }

  VkRenderPassBeginInfo render_pass_info{};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_info.renderPass = render_pass_;
  render_pass_info.framebuffer = framebuffers_[image_index];
  render_pass_info.renderArea.offset = {0, 0};
  render_pass_info.renderArea.extent = extent_;
  vkCmdBeginRenderPass(command_buffer_, &render_pass_info,
                       VK_SUBPASS_CONTENTS_INLINE);

  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command_buffer_);

  vkCmdEndRenderPass(command_buffer_);
  if (vkEndCommandBuffer(command_buffer_) != VK_SUCCESS) {
    SPDLOG_ERROR("Failed to end the ImGui command buffer.");
    return VK_NULL_HANDLE;
  }
  return command_buffer_;
}

void VulkanImGuiBackend::DestroyVulkanResources() {
  if (renderer_ == nullptr) {
    return;
  }
  const VkDevice device = renderer_->Device();
  if (command_pool_ != VK_NULL_HANDLE) {
    vkDestroyCommandPool(device, command_pool_, nullptr);
    command_pool_ = VK_NULL_HANDLE;
    command_buffer_ = VK_NULL_HANDLE;
  }
  for (VkFramebuffer framebuffer : framebuffers_) {
    vkDestroyFramebuffer(device, framebuffer, nullptr);
  }
  framebuffers_.clear();
  if (render_pass_ != VK_NULL_HANDLE) {
    vkDestroyRenderPass(device, render_pass_, nullptr);
    render_pass_ = VK_NULL_HANDLE;
  }
  if (descriptor_pool_ != VK_NULL_HANDLE) {
    if (viewport_descriptor_set_ != VK_NULL_HANDLE) {
      ImGui_ImplVulkan_RemoveTexture(viewport_descriptor_set_);
      viewport_descriptor_set_ = VK_NULL_HANDLE;
    }
    viewport_image_view_ = VK_NULL_HANDLE;
    vkDestroyDescriptorPool(device, descriptor_pool_, nullptr);
    descriptor_pool_ = VK_NULL_HANDLE;
  }
}

bool VulkanImGuiBackend::RebuildFramebuffers() {
  const VkDevice device = renderer_->Device();
  for (VkFramebuffer framebuffer : framebuffers_) {
    vkDestroyFramebuffer(device, framebuffer, nullptr);
  }
  framebuffers_.clear();

  extent_ = renderer_->SwapchainExtent();
  framebuffers_.reserve(renderer_->SwapchainImageCount());
  for (VkImageView image_view : renderer_->SwapchainImageViews()) {
    VkFramebufferCreateInfo framebuffer_info{};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = render_pass_;
    framebuffer_info.attachmentCount = 1;
    framebuffer_info.pAttachments = &image_view;
    framebuffer_info.width = extent_.width;
    framebuffer_info.height = extent_.height;
    framebuffer_info.layers = 1;

    VkFramebuffer framebuffer = VK_NULL_HANDLE;
    if (vkCreateFramebuffer(device, &framebuffer_info, nullptr, &framebuffer) !=
        VK_SUCCESS) {
      SPDLOG_ERROR("Failed to create an ImGui framebuffer.");
      return false;
    }
    framebuffers_.push_back(framebuffer);
  }
  return true;
}

}  // namespace skeleton
