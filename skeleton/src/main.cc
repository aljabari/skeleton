// Copyright 2026 aljabari

#include "libskeleton/opengl/openglrenderer.h"
#include "libskeleton/window.h"

namespace skeleton {

int Run(int argc, char* argv[]) {
  OpenGlRenderer renderer;
  Window window(1280, 720, "Skeleton", renderer);
  while (window.IsOpen()) {
    window.PollEvents();
    renderer.Render();
    window.SwapBuffers();
  }
  return 0;
}

}  // namespace skeleton

int main(int argc, char* argv[]) {
  return skeleton::Run(argc, argv);
}
