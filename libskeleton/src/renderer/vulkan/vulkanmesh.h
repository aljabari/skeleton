// Copyright 2026 aljabari

#ifndef LIBSKELETON_SRC_RENDERER_VULKAN_VULKANMESH_H_
#define LIBSKELETON_SRC_RENDERER_VULKAN_VULKANMESH_H_

#include <volk.h>

#include <cstddef>
#include <vector>

#include "renderer/vulkan/vulkandevice.h"

namespace skeleton {

// RAII wrapper around a Vulkan vertex buffer. Construction creates a
// host-visible, host-coherent VkBuffer backed by VkDeviceMemory and copies
// |vertices| (interleaved position and colour, matching the graphics pipeline's
// vertex layout) into it; destruction frees the memory and destroys the buffer.
// The buffer is host-visible rather than device-local because uploading it
// would need a staging buffer and a command buffer, which the renderer does not
// have yet.
class VulkanMesh {
 public:
  VulkanMesh(const VulkanDevice& device, const std::vector<float>& vertices);
  ~VulkanMesh();

  VulkanMesh(const VulkanMesh&) = delete;
  VulkanMesh& operator=(const VulkanMesh&) = delete;

  VkBuffer Buffer() const;
  std::size_t VertexCount() const;

 private:
  // Returns a memory type index satisfying |type_bits| and |properties|, or
  // throws RendererCreationException when none exists.
  uint32_t FindMemoryType(uint32_t type_bits,
                          VkMemoryPropertyFlags properties) const;
  void CreateVertexBuffer(const std::vector<float>& vertices);

  const VulkanDevice& device_;
  VkBuffer buffer_ = VK_NULL_HANDLE;
  VkDeviceMemory memory_ = VK_NULL_HANDLE;
  std::size_t vertex_count_ = 0;
};

}  // namespace skeleton

#endif  // LIBSKELETON_SRC_RENDERER_VULKAN_VULKANMESH_H_
