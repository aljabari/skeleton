// Copyright 2026 aljabari

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <string>
#include <vector>

#include "skeledit/editorlogsink.h"
#include "skeledit/imguibackend.h"
#include "skeledit/imguibackendfactory.h"
#include "skeledit/imguieditor.h"

#include "libskeleton/logging.h"
#include "libskeleton/renderer.h"
#include "libskeleton/rendererfactory.h"
#include "libskeleton/scene.h"
#include "libskeleton/version.h"
#include "libskeleton/window.h"

namespace skeleton {

namespace {

// Configures spdlog so every log message is written to stdout and to the
// editor's log window. Returns the sink backing the log window.
std::shared_ptr<EditorLogSink> ConfigureLogging() {
  auto log_sink = std::make_shared<EditorLogSink>();
  std::vector<spdlog::sink_ptr> sinks;
  sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
  sinks.push_back(log_sink);
  auto logger = std::make_shared<spdlog::logger>("skeledit", sinks.begin(),
                                                 sinks.end());
  spdlog::set_default_logger(logger);
  spdlog::set_pattern(kLogPatternConsole);
  return log_sink;
}

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
  const std::shared_ptr<EditorLogSink> log_sink = ConfigureLogging();

  const std::string title = std::string("SkelEdit ") + SKELETON_VERSION_STRING;
  auto renderer = CreateRenderer(RenderTarget::kRenderTargetTexture,
                                 WindowConfig{1280, 720, title.c_str()});
  if (renderer == nullptr) {
    SPDLOG_ERROR("No renderer available; exiting.");
    return 1;
  }
  SPDLOG_INFO("SkelEdit {} started.", SKELETON_VERSION_STRING);
  Scene scene = CreateDemoScene();
  Renderer* renderer_ptr = renderer.get();
  Window window(*renderer);
  ImGuiEditor editor(CreateImGuiBackend(window.GetNativeWindow(), renderer_ptr),
                     1280, 720, log_sink,
                     [renderer_ptr](int width, int height) {
                       renderer_ptr->ResizeRenderTarget(width, height);
                     },
                     &scene);
  window.Maximize();

  while (window.IsOpen()) {
    window.PollEvents();
    editor.NewFrame();
    editor.Draw();
    editor.Render();
    renderer->Render(scene);
    window.SwapBuffers();
  }

  return 0;
}

}  // namespace skeleton

int main(int argc, char* argv[]) {
  return skeleton::Run(argc, argv);
}
