// Copyright 2026 aljabari

#ifndef SKELEDIT_IMGUIEDITOR_H_
#define SKELEDIT_IMGUIEDITOR_H_

#include <functional>
#include <memory>

#include <imgui.h>

#include "skeledit/logsink.h"

namespace skeleton {

class ImGuiBackend;

class ImGuiEditor {
 public:
  // |backend| renders the editor UI into the frame of a renderer backend. The
  // backend's Init is called from the constructor after the ImGui context is
  // created; the constructor throws when the backend cannot be initialised.
  // |viewport_texture_id| is the initial ImGui texture identifier of the
  // renderer's render target; set it again each frame with
  // SetViewportTextureId.
  ImGuiEditor(std::unique_ptr<ImGuiBackend> backend, int viewport_width,
              int viewport_height, std::shared_ptr<LogSink> log_sink,
              std::function<void(int, int)> viewport_resize_callback);
  ~ImGuiEditor();

  ImGuiEditor(const ImGuiEditor&) = delete;
  ImGuiEditor& operator=(const ImGuiEditor&) = delete;

  void NewFrame();
  void Draw();
  void Render();

  void SetViewportTextureId(ImTextureID viewport_texture_id);

  static ImGuiID DockspaceId();
  static void CreateDockLayout(int viewport_width, int viewport_height);

 private:
  void DrawDockSpace();
  void DrawViewport();
  void DrawLogs();

  std::unique_ptr<ImGuiBackend> backend_;
  ImTextureID viewport_texture_id_ = 0;
  int viewport_width_;
  int viewport_height_;
  bool dock_layout_initialised_ = false;
  std::shared_ptr<LogSink> log_sink_;
  std::function<void(int, int)> viewport_resize_callback_;
};

}  // namespace skeleton

#endif  // SKELEDIT_IMGUIEDITOR_H_
