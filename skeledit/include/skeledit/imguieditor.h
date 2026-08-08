// Copyright 2026 aljabari

#ifndef SKELEDIT_IMGUIEDITOR_H_
#define SKELEDIT_IMGUIEDITOR_H_

#include <imgui.h>

#include <functional>
#include <memory>

#include <entt/entity/registry.hpp>

#include "skeledit/logsink.h"

namespace skeleton {

class ImGuiBackend;
class Scene;

class ImGuiEditor {
 public:
  // |backend| renders the editor UI into the frame of a renderer backend. The
  // backend's Init is called from the constructor after the ImGui context is
  // created; the constructor throws when the backend cannot be initialised.
  // The viewport shows the texture the backend reports each frame, so a
  // renderer that renders to a texture is previewed there. |viewport_width| and
  // |viewport_height| are the initial viewport size; |viewport_resize_callback|
  // is called when the viewport size changes, and should resize the renderer's
  // render target to match. |scene| is the non-owning scene displayed in the
  // scene graph and entity panels; it must outlive the editor.
  ImGuiEditor(std::unique_ptr<ImGuiBackend> backend, int viewport_width,
              int viewport_height, std::shared_ptr<LogSink> log_sink,
              std::function<void(int, int)> viewport_resize_callback,
              Scene* scene);
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
  void DrawSceneGraph();
  void DrawEntity();
  void DrawLogs();

  std::unique_ptr<ImGuiBackend> backend_;
  int viewport_width_;
  int viewport_height_;
  bool dock_layout_initialised_ = false;
  std::shared_ptr<LogSink> log_sink_;
  std::function<void(int, int)> viewport_resize_callback_;
  Scene* scene_;
  entt::entity selected_entity_ = entt::null;
};

}  // namespace skeleton

#endif  // SKELEDIT_IMGUIEDITOR_H_
