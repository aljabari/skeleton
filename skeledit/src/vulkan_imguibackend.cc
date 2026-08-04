// Copyright 2026 aljabari

#include "skeledit/vulkan_imguibackend.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>
#include <spdlog/spdlog.h>

#include "libskeleton/vulkan/vulkanrenderer.h"

namespace skeleton {

namespace {

// The ImGui Vulkan backend needs at least
// IMGUI_IMPL_VULKAN_MINIMUM_SAMPLED_IMAGE_POOL_SIZE descriptors for the font
// atlas; the extra headroom is for textures the editor adds later (for example
// the render target preview).
constexpr uint32_t kDescriptorPoolSize = 32;

void LogVulkanResult(VkResult result) {
  if (result != VK_SUCCESS) {
    SPDLOG_ERROR("Vulkan call failed inside the ImGui backend ({}).",
                 static_cast<int>(result));
  }
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

  ImGui_ImplVulkan_InitInfo init_info{};
  init_info.ApiVersion = VK_API_VERSION_1_0;
  init_info.Instance = renderer_->Instance();
  init_info.PhysicalDevice = renderer_->PhysicalDevice();
  init_info.Device = renderer_->Device();
  init_info.QueueFamily = renderer_->QueueFamilyIndex();
  init_info.Queue = renderer_->GraphicsQueue();
  init_info.DescriptorPoolSize = kDescriptorPoolSize;
  init_info.MinImageCount = renderer_->SwapchainMinImageCount();
  init_info.ImageCount = renderer_->SwapchainImageCount();
  init_info.CheckVkResultFn = &LogVulkanResult;
  init_info.PipelineInfoMain.RenderPass = renderer_->RenderPass();
  if (!ImGui_ImplVulkan_Init(&init_info)) {
    ImGui_ImplGlfw_Shutdown();
    return false;
  }

  // The ImGui draw data is recorded into the renderer's command buffer inside
  // its render pass, compositing over the triangle on the swapchain image.
  renderer_->SetOverlayDrawCallback(
      [this](VkCommandBuffer command_buffer) {
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command_buffer);
      });
  initialised_ = true;
  return true;
}

void VulkanImGuiBackend::NewFrame() {
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
}

void VulkanImGuiBackend::RenderDrawData() {
  // Drawing happens inside the renderer's render pass via the overlay hook, so
  // there is nothing to do here.
}

void VulkanImGuiBackend::Shutdown() {
  if (renderer_ != nullptr) {
    // Drop the overlay hook before the ImGui backends release their resources
    // so a frame cannot record against a half-destroyed backend.
    renderer_->SetOverlayDrawCallback({});
  }
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  initialised_ = false;
}

}  // namespace skeleton
