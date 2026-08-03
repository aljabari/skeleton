// Copyright 2026 aljabari

#include "libskeleton/renderer.h"

namespace skeleton {

Renderer::Renderer(RenderTarget render_target)
    : render_target_(render_target) {}

Renderer::~Renderer() = default;

unsigned int Renderer::GetTextureId() const {
  return 0;
}

void Renderer::ResizeRenderTarget(int width, int height) {}

}  // namespace skeleton
