// Copyright 2026 aljabari

#include "renderer/vulkan/vulkangraphicspipeline.h"

#include <spdlog/spdlog.h>

#include <cstddef>
#include <fstream>
#include <string>
#include <vector>

#include "libskeleton/renderer.h"

namespace skeleton {

namespace {

std::vector<char> ReadFile(const std::string& path) {
  std::ifstream file(path, std::ios::ate | std::ios::binary);
  if (!file.is_open()) {
    SPDLOG_ERROR("Failed to open shader file \"{}\".", path);
    throw RendererCreationException("Failed to open shader file \"" + path +
                                    "\".");
  }
  const std::streamsize size = file.tellg();
  std::vector<char> buffer(static_cast<std::size_t>(size));
  file.seekg(0);
  file.read(buffer.data(), size);
  return buffer;
}

VkPipelineShaderStageCreateInfo MakeShaderStage(
    VkShaderStageFlagBits stage, VkShaderModule module) {
  VkPipelineShaderStageCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  create_info.stage = stage;
  create_info.module = module;
  create_info.pName = "main";
  return create_info;
}

}  // namespace

VulkanGraphicsPipeline::VulkanGraphicsPipeline(
    const VulkanDevice& device, VkRenderPass render_pass, VkExtent2D extent,
    const std::string& vertex_path, const std::string& fragment_path)
    : device_(device) {
  vertex_shader_ = CreateShaderModule(vertex_path);
  try {
    fragment_shader_ = CreateShaderModule(fragment_path);
    CreatePipelineLayout();
    CreatePipeline(render_pass, extent);
  } catch (...) {
    if (fragment_shader_ != VK_NULL_HANDLE) {
      vkDestroyShaderModule(device_.Device(), fragment_shader_, nullptr);
    }
    if (layout_ != VK_NULL_HANDLE) {
      vkDestroyPipelineLayout(device_.Device(), layout_, nullptr);
    }
    if (pipeline_ != VK_NULL_HANDLE) {
      vkDestroyPipeline(device_.Device(), pipeline_, nullptr);
    }
    vkDestroyShaderModule(device_.Device(), vertex_shader_, nullptr);
    throw;
  }
  SPDLOG_DEBUG("Created Vulkan graphics pipeline.");
}

VulkanGraphicsPipeline::~VulkanGraphicsPipeline() {
  if (pipeline_ != VK_NULL_HANDLE) {
    vkDestroyPipeline(device_.Device(), pipeline_, nullptr);
  }
  if (layout_ != VK_NULL_HANDLE) {
    vkDestroyPipelineLayout(device_.Device(), layout_, nullptr);
  }
  if (vertex_shader_ != VK_NULL_HANDLE) {
    vkDestroyShaderModule(device_.Device(), vertex_shader_, nullptr);
  }
  if (fragment_shader_ != VK_NULL_HANDLE) {
    vkDestroyShaderModule(device_.Device(), fragment_shader_, nullptr);
  }
}

VkPipeline VulkanGraphicsPipeline::Pipeline() const {
  return pipeline_;
}

VkPipelineLayout VulkanGraphicsPipeline::Layout() const {
  return layout_;
}

VkShaderModule VulkanGraphicsPipeline::CreateShaderModule(
    const std::string& path) {
  const std::vector<char> code = ReadFile(path);
  VkShaderModuleCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = code.size();
  create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());
  VkShaderModule shader_module = VK_NULL_HANDLE;
  if (vkCreateShaderModule(device_.Device(), &create_info, nullptr,
                           &shader_module) != VK_SUCCESS) {
    SPDLOG_ERROR("Failed to create the Vulkan shader module for \"{}\".",
                 path);
    throw RendererCreationException(
        "Failed to create a Vulkan shader module.");
  }
  return shader_module;
}

void VulkanGraphicsPipeline::CreatePipelineLayout() {
  VkPipelineLayoutCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  if (vkCreatePipelineLayout(device_.Device(), &create_info, nullptr,
                             &layout_) != VK_SUCCESS) {
    SPDLOG_ERROR("Failed to create the Vulkan pipeline layout.");
    throw RendererCreationException(
        "Failed to create the Vulkan pipeline layout.");
  }
}

void VulkanGraphicsPipeline::CreatePipeline(VkRenderPass render_pass,
                                            VkExtent2D extent) {
  // Interleaved position (location 0) and colour (location 1) attributes,
  // matching the vertex data the mesh supplies: two vec3 floats per vertex.
  VkVertexInputBindingDescription binding{};
  binding.binding = 0;
  binding.stride = 6 * sizeof(float);
  binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  VkVertexInputAttributeDescription attributes[2]{};
  attributes[0].location = 0;
  attributes[0].binding = 0;
  attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  attributes[0].offset = 0;
  attributes[1].location = 1;
  attributes[1].binding = 0;
  attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  attributes[1].offset = 3 * sizeof(float);

  VkPipelineVertexInputStateCreateInfo vertex_input{};
  vertex_input.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input.vertexBindingDescriptionCount = 1;
  vertex_input.pVertexBindingDescriptions = &binding;
  vertex_input.vertexAttributeDescriptionCount = 2;
  vertex_input.pVertexAttributeDescriptions = attributes;

  VkPipelineInputAssemblyStateCreateInfo input_assembly{};
  input_assembly.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(extent.width);
  viewport.height = static_cast<float>(extent.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = extent;

  VkPipelineViewportStateCreateInfo viewport_state{};
  viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state.viewportCount = 1;
  viewport_state.pViewports = &viewport;
  viewport_state.scissorCount = 1;
  viewport_state.pScissors = &scissor;

  VkPipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.sType =
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;
  rasterizer.lineWidth = 1.0f;

  VkPipelineMultisampleStateCreateInfo multisampling{};
  multisampling.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  VkPipelineColorBlendAttachmentState color_blend_attachment{};
  color_blend_attachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachment.blendEnable = VK_FALSE;

  VkPipelineColorBlendStateCreateInfo color_blending{};
  color_blending.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blending.attachmentCount = 1;
  color_blending.pAttachments = &color_blend_attachment;

  VkPipelineShaderStageCreateInfo stages[] = {
      MakeShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vertex_shader_),
      MakeShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fragment_shader_),
  };

  VkGraphicsPipelineCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  create_info.stageCount = 2;
  create_info.pStages = stages;
  create_info.pVertexInputState = &vertex_input;
  create_info.pInputAssemblyState = &input_assembly;
  create_info.pViewportState = &viewport_state;
  create_info.pRasterizationState = &rasterizer;
  create_info.pMultisampleState = &multisampling;
  create_info.pColorBlendState = &color_blending;
  create_info.layout = layout_;
  create_info.renderPass = render_pass;
  create_info.subpass = 0;

  if (vkCreateGraphicsPipelines(device_.Device(), VK_NULL_HANDLE, 1,
                                &create_info, nullptr, &pipeline_) !=
      VK_SUCCESS) {
    SPDLOG_ERROR("Failed to create the Vulkan graphics pipeline.");
    throw RendererCreationException(
        "Failed to create the Vulkan graphics pipeline.");
  }
}

}  // namespace skeleton
