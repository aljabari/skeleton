// Copyright 2026 aljabari

#include <spdlog/spdlog.h>
#include <string>

#include "libskeleton/logging.h"
#include "libskeleton/rendererfactory.h"
#include "libskeleton/version.h"
#include "libskeleton/window.h"

namespace skeleton {

int Run(int argc, char* argv[]) {
  spdlog::set_pattern(kLogPattern);
  const std::string title = std::string("Skeleton ") + SKELETON_VERSION_STRING;
  auto renderer = CreateRenderer(RenderTarget::kRenderTargetWindow,
                                 WindowConfig{1280, 720, title.c_str()});
  if (renderer == nullptr) {
    SPDLOG_ERROR("No renderer available; exiting.");
    return 1;
  }
  SPDLOG_INFO("Skeleton {} started.", SKELETON_VERSION_STRING);
  Window window(*renderer);
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
