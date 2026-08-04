// Copyright 2026 aljabari

#include "renderer/vulkan/vulkanmesh.h"

#include <spdlog/spdlog.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

#include "libskeleton/renderer.h"

namespace skeleton {

namespace {

// Floats per vertex in the interleaved position (vec3) + colour (vec3) layout
// shared with the OpenGL mesh.
constexpr std::size_t kFloatsPerVertex = 6;

}  // namespace

VulkanMesh::VulkanMesh(const VulkanDevice& device,
                       const std::vector<float>& vertices)
    : device_(device),
      vertex_count_(vertices.size() / kFloatsPerVertex) {
  try {
    CreateVertexBuffer(vertices);
  } catch (...) {
    if (memory_ != VK_NULL_HANDLE) {
      vkFreeMemory(device_.Device(), memory_, nullptr);
    }
    if (buffer_ != VK_NULL_HANDLE) {
      vkDestroyBuffer(device_.Device(), buffer_, nullptr);
    }
    throw;
  }
  SPDLOG_DEBUG("Created Vulkan mesh with {} vertices.", vertex_count_);
}

VulkanMesh::~VulkanMesh() {
  if (memory_ != VK_NULL_HANDLE) {
    vkFreeMemory(device_.Device(), memory_, nullptr);
  }
  if (buffer_ != VK_NULL_HANDLE) {
    vkDestroyBuffer(device_.Device(), buffer_, nullptr);
  }
}

VkBuffer VulkanMesh::Buffer() const {
  return buffer_;
}

std::size_t VulkanMesh::VertexCount() const {
  return vertex_count_;
}

uint32_t VulkanMesh::FindMemoryType(
    uint32_t type_bits, VkMemoryPropertyFlags properties) const {
  VkPhysicalDeviceMemoryProperties memory_properties{};
  vkGetPhysicalDeviceMemoryProperties(device_.PhysicalDevice(),
                                      &memory_properties);
  for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i) {
    if ((type_bits & (1u << i)) != 0 &&
        (memory_properties.memoryTypes[i].propertyFlags & properties) ==
            properties) {
      return i;
    }
  }
  SPDLOG_ERROR("Failed to find a suitable Vulkan memory type.");
  throw RendererCreationException(
      "Failed to find a suitable Vulkan memory type.");
}

void VulkanMesh::CreateVertexBuffer(const std::vector<float>& vertices) {
  const VkDeviceSize size = vertices.size() * sizeof(float);
  if (size == 0) {
    SPDLOG_ERROR("Cannot create a Vulkan vertex buffer with no vertices.");
    throw RendererCreationException(
        "Cannot create an empty Vulkan vertex buffer.");
  }

  VkBufferCreateInfo buffer_info{};
  buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_info.size = size;
  buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  if (vkCreateBuffer(device_.Device(), &buffer_info, nullptr, &buffer_) !=
      VK_SUCCESS) {
    SPDLOG_ERROR("Failed to create the Vulkan vertex buffer.");
    throw RendererCreationException(
        "Failed to create the Vulkan vertex buffer.");
  }

  VkMemoryRequirements memory_requirements{};
  vkGetBufferMemoryRequirements(device_.Device(), buffer_,
                                &memory_requirements);
  VkMemoryAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.allocationSize = memory_requirements.size;
  alloc_info.memoryTypeIndex = FindMemoryType(
      memory_requirements.memoryTypeBits,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  if (vkAllocateMemory(device_.Device(), &alloc_info, nullptr, &memory_) !=
      VK_SUCCESS) {
    SPDLOG_ERROR("Failed to allocate memory for the Vulkan vertex buffer.");
    throw RendererCreationException(
        "Failed to allocate memory for the Vulkan vertex buffer.");
  }

  if (vkBindBufferMemory(device_.Device(), buffer_, memory_, 0) != VK_SUCCESS) {
    SPDLOG_ERROR("Failed to bind memory to the Vulkan vertex buffer.");
    throw RendererCreationException(
        "Failed to bind memory to the Vulkan vertex buffer.");
  }

  void* data = nullptr;
  if (vkMapMemory(device_.Device(), memory_, 0, size, 0, &data) != VK_SUCCESS) {
    SPDLOG_ERROR("Failed to map the Vulkan vertex buffer memory.");
    throw RendererCreationException(
        "Failed to map the Vulkan vertex buffer memory.");
  }
  std::memcpy(data, vertices.data(), size);
  vkUnmapMemory(device_.Device(), memory_);
}

}  // namespace skeleton
