// Copyright 2026 aljabari

#include "libskeleton/rendererfactory.h"

#include <memory>

#include "libskeleton/opengl/openglrenderer.h"
#include "libskeleton/vulkan/vulkanrenderer.h"

namespace skeleton {

namespace {

// Creates a single renderer for |backend|, or nullptr when the backend is not
// available on the current platform. Renderers whose construction fails throw
// RendererCreationException.
std::unique_ptr<Renderer> CreatePlatformRenderer(RendererBackend backend,
                                                 bool render_to_texture) {
  switch (backend) {
    case RendererBackend::kVulkan:
#ifdef SKELETON_TARGET_SUPPORTS_RENDERER_VULKAN
      return std::make_unique<VulkanRenderer>();
#else
      return nullptr;
#endif
    case RendererBackend::kOpenGl:
#ifdef SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL
      return std::make_unique<OpenGlRenderer>(render_to_texture);
#else
      return nullptr;
#endif
  }
  return nullptr;
}

// Creates the renderer for a single backend through |creators|, or nullptr
// when the backend has no creator, its creator returns nullptr, or its creator
// throws RendererCreationException.
std::unique_ptr<Renderer> CreateBackend(const RendererCreatorMap& creators,
                                        RendererBackend backend,
                                        bool render_to_texture) {
  auto creator = creators.find(backend);
  if (creator == creators.end()) {
    return nullptr;
  }
  try {
    return creator->second(render_to_texture);
  } catch (const RendererCreationException&) {
    // The backend failed to initialise, so fall back to the next one.
    return nullptr;
  }
}

// The real creators for the current platform.
const RendererCreatorMap& PlatformCreators() {
  static const RendererCreatorMap creators = {
      {RendererBackend::kVulkan,
       [](bool render_to_texture) {
         return CreatePlatformRenderer(RendererBackend::kVulkan,
                                       render_to_texture);
       }},
      {RendererBackend::kOpenGl,
       [](bool render_to_texture) {
         return CreatePlatformRenderer(RendererBackend::kOpenGl,
                                       render_to_texture);
       }},
  };
  return creators;
}

}  // namespace

const RendererPriorityList& RendererPriorityOrder() {
  static const RendererPriorityList priority_order = {
      // Add new backends here in platform preference order. For example, a
      // future DirectX backend on Windows would be listed before Vulkan.
#ifdef SKELETON_TARGET_SUPPORTS_RENDERER_VULKAN
      RendererBackend::kVulkan,
#endif
#ifdef SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL
      RendererBackend::kOpenGl,
#endif
  };
  return priority_order;
}

std::unique_ptr<Renderer> CreateRendererWithFallback(
    RendererBackend preferred, const RendererPriorityList& priority_order,
    const RendererCreatorMap& creators, bool render_to_texture) {
  if (std::unique_ptr<Renderer> renderer =
          CreateBackend(creators, preferred, render_to_texture)) {
    return renderer;
  }

  for (RendererBackend backend : priority_order) {
    if (backend == preferred) {
      continue;
    }
    if (std::unique_ptr<Renderer> renderer =
            CreateBackend(creators, backend, render_to_texture)) {
      return renderer;
    }
  }

  return nullptr;
}

std::unique_ptr<Renderer> CreateRendererWithFallback(
    RendererBackend preferred, bool render_to_texture) {
  return CreateRendererWithFallback(preferred, RendererPriorityOrder(),
                                    PlatformCreators(), render_to_texture);
}

std::unique_ptr<Renderer> CreateRenderer(bool render_to_texture) {
  const RendererPriorityList& priority_order = RendererPriorityOrder();
  const RendererCreatorMap& creators = PlatformCreators();
  for (RendererBackend backend : priority_order) {
    if (std::unique_ptr<Renderer> renderer =
            CreateBackend(creators, backend, render_to_texture)) {
      return renderer;
    }
  }
  return nullptr;
}

}  // namespace skeleton
