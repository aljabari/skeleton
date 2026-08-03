// Copyright 2026 aljabari

#include <spdlog/spdlog.h>

#include "libskeleton/rendererfactory.h"
#include "libskeleton/window.h"

namespace skeleton {

int Run(int argc, char* argv[]) {
  auto renderer = CreateRenderer();
  if (renderer == nullptr) {
    spdlog::error("No renderer available; exiting.");
    return 1;
  }
  spdlog::info("Skeleton started.");
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
