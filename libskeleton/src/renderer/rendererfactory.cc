// Copyright 2026 aljabari

#include "libskeleton/rendererfactory.h"

#include <spdlog/spdlog.h>

#include <memory>

#include "libskeleton/opengl/openglrenderer.h"
#include "libskeleton/vulkan/vulkanrenderer.h"

namespace skeleton {

namespace {

// Human-readable name for a backend, used in log messages.
const char* BackendName(RendererBackend backend) {
  switch (backend) {
    case RendererBackend::kVulkan:
      return "Vulkan";
    case RendererBackend::kOpenGl:
      return "OpenGL";
  }
  return "Unknown";
}

// Creates a single renderer for |backend|, or nullptr when the backend is not
// available on the current platform. Renderers whose construction fails throw
// RendererCreationException.
std::unique_ptr<Renderer> CreatePlatformRenderer(RendererBackend backend,
                                                 RenderTarget render_target) {
  switch (backend) {
    case RendererBackend::kVulkan:
#ifdef SKELETON_TARGET_SUPPORTS_RENDERER_VULKAN
      return std::make_unique<VulkanRenderer>();
#else
      spdlog::warn("Renderer backend {} is not supported on this platform.",
                   BackendName(backend));
      return nullptr;
#endif
    case RendererBackend::kOpenGl:
#ifdef SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL
      return std::make_unique<OpenGlRenderer>(render_target);
#else
      spdlog::warn("Renderer backend {} is not supported on this platform.",
                   BackendName(backend));
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
                                        RenderTarget render_target) {
  auto creator = creators.find(backend);
  if (creator == creators.end()) {
    spdlog::debug("Renderer backend {} has no registered creator; skipping.",
                  BackendName(backend));
    return nullptr;
  }
  try {
    return creator->second(render_target);
  } catch (const RendererCreationException& exception) {
    // The backend failed to initialise, so fall back to the next one.
    spdlog::warn("Renderer backend {} failed to initialise: {}",
                 BackendName(backend), exception.what());
    return nullptr;
  }
}

// The real creators for the current platform.
const RendererCreatorMap& PlatformCreators() {
  static const RendererCreatorMap creators = {
      {RendererBackend::kVulkan,
       [](RenderTarget render_target) {
         return CreatePlatformRenderer(RendererBackend::kVulkan,
                                       render_target);
       }},
      {RendererBackend::kOpenGl,
       [](RenderTarget render_target) {
         return CreatePlatformRenderer(RendererBackend::kOpenGl,
                                       render_target);
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
    const RendererCreatorMap& creators, RenderTarget render_target) {
  if (std::unique_ptr<Renderer> renderer =
          CreateBackend(creators, preferred, render_target)) {
    spdlog::info("Created renderer backend {}.",
                 BackendName(renderer->GetBackend()));
    return renderer;
  }
  spdlog::info("Preferred renderer backend {} is unavailable; falling back.",
               BackendName(preferred));

  for (RendererBackend backend : priority_order) {
    if (backend == preferred) {
      continue;
    }
    if (std::unique_ptr<Renderer> renderer =
            CreateBackend(creators, backend, render_target)) {
      spdlog::info("Created renderer backend {}.",
                   BackendName(renderer->GetBackend()));
      return renderer;
    }
  }

  spdlog::error("Failed to create any renderer backend.");
  return nullptr;
}

std::unique_ptr<Renderer> CreateRendererWithFallback(
    RendererBackend preferred, RenderTarget render_target) {
  return CreateRendererWithFallback(preferred, RendererPriorityOrder(),
                                    PlatformCreators(), render_target);
}

std::unique_ptr<Renderer> CreateRenderer(RenderTarget render_target) {
  const RendererPriorityList& priority_order = RendererPriorityOrder();
  const RendererCreatorMap& creators = PlatformCreators();
  for (RendererBackend backend : priority_order) {
    if (std::unique_ptr<Renderer> renderer =
            CreateBackend(creators, backend, render_target)) {
      spdlog::info("Created renderer backend {}.",
                   BackendName(renderer->GetBackend()));
      return renderer;
    }
  }
  spdlog::error("Failed to create any renderer backend.");
  return nullptr;
}

}  // namespace skeleton
