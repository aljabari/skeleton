// Copyright 2026 aljabari

#include <memory>

#include "skeledit/imguieditor.h"

#include "libskeleton/renderer.h"
#include "libskeleton/rendererfactory.h"
#include "libskeleton/window.h"

namespace skeleton {

int Run(int argc, char* argv[]) {
  auto renderer = CreateRendererWithFallback(RendererBackend::kOpenGl, true);
  if (renderer == nullptr) {
    return 1;
  }
  Renderer* renderer_ptr = renderer.get();
  Window window(1280, 720, "SkelEdit", *renderer);
  ImGuiEditor editor(window.GetNativeWindow(), renderer_ptr->GetTextureId(),
                     1280, 720, [renderer_ptr](int width, int height) {
                       renderer_ptr->ResizeRenderTarget(width, height);
                     });
  window.Maximize();

  while (window.IsOpen()) {
    window.PollEvents();
    editor.NewFrame();
    editor.SetViewportTextureId(renderer_ptr->GetTextureId());
    editor.Draw();
    renderer->Render();
    editor.Render();
    window.SwapBuffers();
  }

  return 0;
}

}  // namespace skeleton

int main(int argc, char* argv[]) {
  return skeleton::Run(argc, argv);
}
