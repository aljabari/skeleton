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
      SPDLOG_WARN("Renderer backend {} is not supported on this platform.",
                   BackendName(backend));
      return nullptr;
#endif
    case RendererBackend::kOpenGl:
#ifdef SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL
      return std::make_unique<OpenGlRenderer>(render_target);
#else
      SPDLOG_WARN("Renderer backend {} is not supported on this platform.",
                   BackendName(backend));
      return nullptr;
#endif
  }
  return nullptr;
}

// Creates the renderer for a single backend through |creators|, or nullptr
// when the backend has no creator, its creator returns nullptr, or its creator
// or the renderer's context creation throws RendererCreationException. The
// renderer's CreateContext is called here so window creation and context
// initialisation failures trigger fallback.
std::unique_ptr<Renderer> CreateBackend(const RendererCreatorMap& creators,
                                        RendererBackend backend,
                                        RenderTarget render_target,
                                        const WindowConfig& window_config) {
  auto creator = creators.find(backend);
  if (creator == creators.end()) {
    SPDLOG_DEBUG("Renderer backend {} has no registered creator; skipping.",
                  BackendName(backend));
    return nullptr;
  }
  try {
    std::unique_ptr<Renderer> renderer = creator->second(render_target);
    if (renderer != nullptr) {
      renderer->CreateContext(window_config);
    }
    return renderer;
  } catch (const RendererCreationException& exception) {
    // The backend failed to initialise, so fall back to the next one.
    SPDLOG_WARN("Renderer backend {} failed to initialise: {}",
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

std::unique_ptr<Renderer> CreateRenderer(
    RendererBackend preferred, const RendererPriorityList& priority_order,
    const RendererCreatorMap& creators, RenderTarget render_target,
    const WindowConfig& window_config) {
  if (std::unique_ptr<Renderer> renderer = CreateBackend(
          creators, preferred, render_target, window_config)) {
    SPDLOG_INFO("Created renderer backend {}.",
                 BackendName(renderer->GetBackend()));
    return renderer;
  }
  SPDLOG_INFO("Preferred renderer backend {} is unavailable; falling back.",
               BackendName(preferred));

  for (RendererBackend backend : priority_order) {
    if (backend == preferred) {
      continue;
    }
    if (std::unique_ptr<Renderer> renderer =
            CreateBackend(creators, backend, render_target, window_config)) {
      SPDLOG_INFO("Created renderer backend {}.",
                   BackendName(renderer->GetBackend()));
      return renderer;
    }
  }

  SPDLOG_ERROR("Failed to create any renderer backend.");
  return nullptr;
}

std::unique_ptr<Renderer> CreateRenderer(RendererBackend preferred,
                                         RenderTarget render_target,
                                         const WindowConfig& window_config) {
  return CreateRenderer(preferred, RendererPriorityOrder(), PlatformCreators(),
                        render_target, window_config);
}

std::unique_ptr<Renderer> CreateRenderer(RenderTarget render_target,
                                         const WindowConfig& window_config) {
  const RendererPriorityList& priority_order = RendererPriorityOrder();
  const RendererCreatorMap& creators = PlatformCreators();
  for (RendererBackend backend : priority_order) {
    if (std::unique_ptr<Renderer> renderer =
            CreateBackend(creators, backend, render_target, window_config)) {
      SPDLOG_INFO("Created renderer backend {}.",
                   BackendName(renderer->GetBackend()));
      return renderer;
    }
  }
  SPDLOG_ERROR("Failed to create any renderer backend.");
  return nullptr;
}

}  // namespace skeleton
