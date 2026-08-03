// Copyright 2026 aljabari

#include "libskeleton/rendererfactory.h"
#include "libskeleton/window.h"

namespace skeleton {

int Run(int argc, char* argv[]) {
  auto renderer = CreateRendererWithFallback(RendererBackend::kVulkan);
  if (renderer == nullptr) {
    return 1;
  }
  Window window(1280, 720, "Skeleton", *renderer);
  while (window.IsOpen()) {
    window.PollEvents();
    renderer->Render();
    window.SwapBuffers();
  }
  return 0;
}

}  // namespace skeleton

int main(int argc, char* argv[]) {
  return skeleton::Run(argc, argv);
}
