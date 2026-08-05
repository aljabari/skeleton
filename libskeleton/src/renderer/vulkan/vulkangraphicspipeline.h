// Copyright 2026 aljabari

#ifndef LIBSKELETON_SRC_RENDERER_VULKAN_VULKANGRAPHICSPIPELINE_H_
#define LIBSKELETON_SRC_RENDERER_VULKAN_VULKANGRAPHICSPIPELINE_H_

#include <volk.h>

#include <string>

#include "renderer/vulkan/vulkandevice.h"

namespace skeleton {

// RAII wrapper around a Vulkan graphics pipeline for the triangle mesh. The
// vertex and fragment shaders are read as pre-compiled SPIR-V files (produced
// at build time from the GLSL sources under libskeleton/res/shaders), a shader
// module is created for each, and a pipeline with a single triangle-list vertex
// layout (interleaved position and colour) is created against |render_pass|
// with dynamic viewport and scissor state, so the same pipeline can draw into
// framebuffers of different sizes (the swapchain and the off-screen render
// target). Construction throws RendererCreationException on failure;
// destruction destroys the pipeline, its layout, and the shader modules.
class VulkanGraphicsPipeline {
 public:
  VulkanGraphicsPipeline(const VulkanDevice& device, VkRenderPass render_pass,
                         const std::string& vertex_path,
                         const std::string& fragment_path);
  ~VulkanGraphicsPipeline();

  VulkanGraphicsPipeline(const VulkanGraphicsPipeline&) = delete;
  VulkanGraphicsPipeline& operator=(const VulkanGraphicsPipeline&) = delete;

  VkPipeline Pipeline() const;
  VkPipelineLayout Layout() const;

 private:
  VkShaderModule CreateShaderModule(const std::string& path);
  void CreatePipelineLayout();
  void CreatePipeline(VkRenderPass render_pass);

  const VulkanDevice& device_;
  VkShaderModule vertex_shader_ = VK_NULL_HANDLE;
  VkShaderModule fragment_shader_ = VK_NULL_HANDLE;
  VkPipelineLayout layout_ = VK_NULL_HANDLE;
  VkPipeline pipeline_ = VK_NULL_HANDLE;
};

}  // namespace skeleton

#endif  // LIBSKELETON_SRC_RENDERER_VULKAN_VULKANGRAPHICSPIPELINE_H_
