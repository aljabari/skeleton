// Copyright 2026 aljabari

#include <spdlog/spdlog.h>

#include <string>
#include <vector>

#include "libskeleton/logging.h"
#include "libskeleton/rendererfactory.h"
#include "libskeleton/scene.h"
#include "libskeleton/version.h"
#include "libskeleton/window.h"

namespace skeleton {

namespace {

// The demo triangle, authored in the Vulkan coordinate system (front faces
// wind counter-clockwise in the y-down framebuffer): three vertices of
// interleaved position (vec3) and colour (vec3).
Scene CreateDemoScene() {
  Scene scene;
  const entt::entity triangle = scene.Registry().create();
  scene.Registry().emplace<NameComponent>(triangle, "Triangle");
  scene.Registry().emplace<MeshComponent>(
      triangle, std::vector<float>{
                    -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  //
                    0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f,  //
                    0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  //
                });
  return scene;
}

}  // namespace

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
  Scene scene = CreateDemoScene();
  Window window(*renderer);
  while (window.IsOpen()) {
    window.PollEvents();
    renderer->Render(scene);
    window.SwapBuffers();
  }
  return 0;
}

}  // namespace skeleton

int main(int argc, char* argv[]) {
  return skeleton::Run(argc, argv);
}
