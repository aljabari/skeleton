// Copyright 2026 aljabari

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <vector>

#include "skeledit/editorlogsink.h"
#include "skeledit/imguibackend.h"
#include "skeledit/imguibackendfactory.h"
#include "skeledit/imguieditor.h"

#include "libskeleton/logging.h"
#include "libskeleton/renderer.h"
#include "libskeleton/rendererfactory.h"
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

}  // namespace

int Run(int argc, char* argv[]) {
  const std::shared_ptr<EditorLogSink> log_sink = ConfigureLogging();

  auto renderer = CreateRenderer(RenderTarget::kRenderTargetTexture,
                                 WindowConfig{1280, 720, "SkelEdit"});
  if (renderer == nullptr) {
    SPDLOG_ERROR("No renderer available; exiting.");
    return 1;
  }
  SPDLOG_INFO("SkelEdit started.");
  Renderer* renderer_ptr = renderer.get();
  Window window(*renderer);
  ImGuiEditor editor(CreateImGuiBackend(window.GetNativeWindow(), renderer_ptr),
                     1280, 720, log_sink,
                     [renderer_ptr](int width, int height) {
                       renderer_ptr->ResizeRenderTarget(width, height);
                     });
  window.Maximize();

  while (window.IsOpen()) {
    window.PollEvents();
    editor.NewFrame();
    editor.Draw();
    if (renderer_ptr->GetBackend() == RendererBackend::kVulkan) {
      // The Vulkan renderer submits the ImGui command buffer through the frame
      // submit hook after recording its own frame, so the draw data must be
      // produced before the renderer records it.
      editor.Render();
      renderer->Render();
    } else {
      renderer->Render();
      editor.Render();
    }
    window.SwapBuffers();
  }

  return 0;
}

}  // namespace skeleton

int main(int argc, char* argv[]) {
  return skeleton::Run(argc, argv);
}
