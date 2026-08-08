// Copyright 2026 aljabari

#include <spdlog/spdlog.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#endif

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

#if defined(__EMSCRIPTEN__)
// Emscripten drives the frame loop from the browser's event loop via
// emscripten_set_main_loop, which unwinds main()'s stack (it throws a
// sentinel exception the runtime catches). The loop state must therefore
// outlive Run() in static storage.
std::unique_ptr<Renderer> g_loop_renderer;
std::unique_ptr<Window> g_loop_window;
Scene g_loop_scene;

void EmscriptenFrameIteration() {
  if (!g_loop_window->IsOpen()) {
    emscripten_cancel_main_loop();
    return;
  }
  g_loop_window->PollEvents();
  g_loop_renderer->Render(g_loop_scene);
  g_loop_window->SwapBuffers();
}
#endif

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
#if defined(__EMSCRIPTEN__)
  // A blocking render loop would stall the browser's event loop (the page
  // becomes unresponsive and frames are never presented). Hand the frame loop
  // to emscripten_set_main_loop (requestAnimationFrame-driven) and let main()
  // return; the loop state lives in static storage above.
  g_loop_renderer = std::move(renderer);
  g_loop_window = std::make_unique<Window>(*g_loop_renderer);
  g_loop_scene = std::move(scene);
  emscripten_set_main_loop(EmscriptenFrameIteration, 0, 1);
  return 0;
#else
  Window window(*renderer);
  while (window.IsOpen()) {
    window.PollEvents();
    renderer->Render(scene);
    window.SwapBuffers();
  }
  return 0;
#endif
}

}  // namespace skeleton

int main(int argc, char* argv[]) {
  return skeleton::Run(argc, argv);
}
