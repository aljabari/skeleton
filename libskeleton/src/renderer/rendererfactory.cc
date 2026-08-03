// Copyright 2026 aljabari

#include "libskeleton/rendererfactory.h"

#include <memory>

#include "libskeleton/opengl/openglrenderer.h"

namespace skeleton {

namespace {

// Creates a single renderer for |backend|, or nullptr when the backend is not
// available on the current platform.
std::unique_ptr<Renderer> CreateRenderer(RendererBackend backend,
                                         bool render_to_texture) {
  switch (backend) {
    case RendererBackend::kVulkan:
      // The Vulkan renderer is not implemented yet, so creating it always
      // fails and the factory falls back to the next backend in priority
      // order.
      return nullptr;
    case RendererBackend::kOpenGl:
#ifdef SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL
      return std::make_unique<OpenGlRenderer>(render_to_texture);
#else
      return nullptr;
#endif
  }
  return nullptr;
}

// The real creators for the current platform.
const RendererCreatorMap& PlatformCreators() {
  static const RendererCreatorMap creators = {
      {RendererBackend::kVulkan,
       [](bool render_to_texture) {
         return CreateRenderer(RendererBackend::kVulkan, render_to_texture);
       }},
      {RendererBackend::kOpenGl,
       [](bool render_to_texture) {
         return CreateRenderer(RendererBackend::kOpenGl, render_to_texture);
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
  auto create = [&creators, render_to_texture](
                    RendererBackend backend) -> std::unique_ptr<Renderer> {
    auto creator = creators.find(backend);
    if (creator == creators.end()) {
      return nullptr;
    }
    return creator->second(render_to_texture);
  };

  if (std::unique_ptr<Renderer> renderer = create(preferred)) {
    return renderer;
  }

  for (RendererBackend backend : priority_order) {
    if (backend == preferred) {
      continue;
    }
    if (std::unique_ptr<Renderer> renderer = create(backend)) {
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

}  // namespace skeleton
