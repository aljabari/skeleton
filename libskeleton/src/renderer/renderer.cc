// Copyright 2026 aljabari

#include "libskeleton/renderer.h"

namespace skeleton {

Renderer::Renderer(bool render_to_texture)
    : render_to_texture_(render_to_texture) {}

Renderer::~Renderer() = default;

}  // namespace skeleton
