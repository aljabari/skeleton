// Copyright 2026 aljabari

#ifndef LIBSKELETON_RENDERERFACTORY_H_
#define LIBSKELETON_RENDERERFACTORY_H_

#include <functional>
#include <map>
#include <memory>
#include <vector>

#include "libskeleton/renderer.h"

namespace skeleton {

// Creates a renderer for a backend, or nullptr when the backend is unavailable
// on the current platform. A creator may throw RendererCreationException to
// report that the backend failed to initialise.
using RendererCreator = std::function<std::unique_ptr<Renderer>(RenderTarget)>;

// Maps each backend to the function that creates its renderer.
using RendererCreatorMap = std::map<RendererBackend, RendererCreator>;

// Backends ordered by fallback priority, first = most preferred.
using RendererPriorityList = std::vector<RendererBackend>;

// Returns the fallback priority order for the current platform. Backends
// earlier in the list are preferred over later ones. The backend explicitly
// requested by the caller is always tried first, regardless of this order.
const RendererPriorityList& RendererPriorityOrder();

// Creates a renderer, trying |preferred| first and falling back through
// |priority_order| when a backend cannot be created. A backend is skipped when
// it has no entry in |creators|, its creator returns nullptr, or its creator
// throws RendererCreationException. Returns nullptr when every candidate
// backend fails.
std::unique_ptr<Renderer> CreateRendererWithFallback(
    RendererBackend preferred, const RendererPriorityList& priority_order,
    const RendererCreatorMap& creators,
    RenderTarget render_target = RenderTarget::kRenderTargetWindow);

// Convenience overload that uses the platform's priority order and real
// backend creators.
std::unique_ptr<Renderer> CreateRendererWithFallback(
    RendererBackend preferred,
    RenderTarget render_target = RenderTarget::kRenderTargetWindow);

// Creates a renderer using the platform's priority order and real backend
// creators, without a preferred backend. The first backend in priority order
// that can be created wins. Returns nullptr when every backend fails.
std::unique_ptr<Renderer> CreateRenderer(
    RenderTarget render_target = RenderTarget::kRenderTargetWindow);

}  // namespace skeleton

#endif  // LIBSKELETON_RENDERERFACTORY_H_
