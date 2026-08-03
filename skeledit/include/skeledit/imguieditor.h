// Copyright 2026 aljabari

#ifndef SKELEDIT_IMGUIEDITOR_H_
#define SKELEDIT_IMGUIEDITOR_H_

#include <imgui.h>

struct GLFWwindow;

namespace skeleton {

class ImGuiEditor {
 public:
  ImGuiEditor(GLFWwindow* window, unsigned int viewport_texture_id,
              int viewport_width, int viewport_height);
  ~ImGuiEditor();

  ImGuiEditor(const ImGuiEditor&) = delete;
  ImGuiEditor& operator=(const ImGuiEditor&) = delete;

  void NewFrame();
  void Draw();
  void Render();

  static ImGuiID DockspaceId();
  static void CreateDockLayout(int viewport_width, int viewport_height);

 private:
  void DrawDockSpace();
  void DrawViewport();

  GLFWwindow* window_;
  unsigned int viewport_texture_id_;
  int viewport_width_;
  int viewport_height_;
  bool dock_layout_initialised_ = false;
};

}  // namespace skeleton

#endif  // SKELEDIT_IMGUIEDITOR_H_
