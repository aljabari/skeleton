// Copyright 2026 aljabari

#include "skeledit/imguibackendfactory.h"

#include <memory>

#ifdef SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL
#include "libskeleton/opengl/openglrenderer.h"
#endif
#ifdef SKELETON_TARGET_SUPPORTS_RENDERER_VULKAN
#include "libskeleton/vulkan/vulkanrenderer.h"
#endif
#include "skeledit/imguibackend.h"
#ifdef SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL
#include "skeledit/opengl_imguibackend.h"
#endif
#ifdef SKELETON_TARGET_SUPPORTS_RENDERER_VULKAN
#include "skeledit/vulkan_imguibackend.h"
#endif

namespace skeleton {

std::unique_ptr<ImGuiBackend> CreateImGuiBackend(GLFWwindow* window,
                                                 Renderer* renderer) {
  if (renderer == nullptr) {
    return nullptr;
  }
  switch (renderer->GetBackend()) {
    case RendererBackend::kVulkan:
#ifdef SKELETON_TARGET_SUPPORTS_RENDERER_VULKAN
      return std::make_unique<VulkanImGuiBackend>(
          window, dynamic_cast<VulkanRenderer*>(renderer));
#else
      return nullptr;
#endif
    case RendererBackend::kOpenGl:
#ifdef SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL
      return std::make_unique<OpenGlImGuiBackend>(window, renderer);
#else
      return nullptr;
#endif
  }
  return nullptr;
}

}  // namespace skeleton
