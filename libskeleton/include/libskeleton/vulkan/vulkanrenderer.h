// Copyright 2026 aljabari

#ifndef LIBSKELETON_VULKAN_VULKANRENDERER_H_
#define LIBSKELETON_VULKAN_VULKANRENDERER_H_

#include "libskeleton/renderer.h"

struct GLFWwindow;

namespace skeleton {

class VulkanRenderer : public Renderer {
 public:
  VulkanRenderer();
  ~VulkanRenderer() override;

  void SetWindowHints() override;
  void InitialiseForWindow(GLFWwindow* window) override;
  void Render() override;
};

}  // namespace skeleton

#endif  // LIBSKELETON_VULKAN_VULKANRENDERER_H_
