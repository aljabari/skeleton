// Copyright 2026 aljabari

#ifndef SKELEDIT_IMGUIEDITOR_H_
#define SKELEDIT_IMGUIEDITOR_H_

#include <functional>
#include <memory>

#include <imgui.h>

#include "skeledit/logsink.h"

struct GLFWwindow;

namespace skeleton {

class ImGuiEditor {
 public:
  ImGuiEditor(GLFWwindow* window, unsigned int viewport_texture_id,
              int viewport_width, int viewport_height,
              std::shared_ptr<LogSink> log_sink,
              std::function<void(int, int)> viewport_resize_callback);
  ~ImGuiEditor();

  ImGuiEditor(const ImGuiEditor&) = delete;
  ImGuiEditor& operator=(const ImGuiEditor&) = delete;

  void NewFrame();
  void Draw();
  void Render();

  void SetViewportTextureId(unsigned int viewport_texture_id);

  static ImGuiID DockspaceId();
  static void CreateDockLayout(int viewport_width, int viewport_height);

 private:
  void DrawDockSpace();
  void DrawViewport();
  void DrawLogs();

  GLFWwindow* window_;
  unsigned int viewport_texture_id_;
  int viewport_width_;
  int viewport_height_;
  bool dock_layout_initialised_ = false;
  std::shared_ptr<LogSink> log_sink_;
  std::function<void(int, int)> viewport_resize_callback_;
};

}  // namespace skeleton

#endif  // SKELEDIT_IMGUIEDITOR_H_
