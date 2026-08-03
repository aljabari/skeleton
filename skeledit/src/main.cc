// Copyright 2026 aljabari

#include "skeledit/imguieditor.h"

#include "libskeleton/opengl/openglrenderer.h"
#include "libskeleton/window.h"

namespace skeleton {

int Run(int argc, char* argv[]) {
  OpenGlRenderer renderer(true);
  Window window(1280, 720, "SkelEdit", renderer);
  ImGuiEditor editor(
      window.GetNativeWindow(), renderer.GetTextureId(), 1280, 720,
      [&renderer](int width, int height) {
        renderer.ResizeRenderTarget(width, height);
      });
  window.Maximize();

  while (window.IsOpen()) {
    window.PollEvents();
    editor.NewFrame();
    editor.SetViewportTextureId(renderer.GetTextureId());
    editor.Draw();
    renderer.Render();
    editor.Render();
    window.SwapBuffers();
  }

  return 0;
}

}  // namespace skeleton

int main(int argc, char* argv[]) {
  return skeleton::Run(argc, argv);
}
