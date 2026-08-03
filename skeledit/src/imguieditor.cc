// Copyright 2026 aljabari

#include "skeledit/imguieditor.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>
#include <imgui_internal.h>

#include <utility>

namespace skeleton {

ImGuiEditor::ImGuiEditor(GLFWwindow* window, unsigned int viewport_texture_id,
                         int viewport_width, int viewport_height,
                         std::function<void(int, int)> viewport_resize_callback)
    : window_(window),
      viewport_texture_id_(viewport_texture_id),
      viewport_width_(viewport_width),
      viewport_height_(viewport_height),
      viewport_resize_callback_(std::move(viewport_resize_callback)) {
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.IniFilename = nullptr;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  ImGui_ImplGlfw_InitForOpenGL(window_, true);
  ImGui_ImplOpenGL3_Init("#version 130");
}

ImGuiEditor::~ImGuiEditor() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void ImGuiEditor::NewFrame() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void ImGuiEditor::Draw() {
  if (!dock_layout_initialised_) {
    CreateDockLayout(viewport_width_, viewport_height_);
    dock_layout_initialised_ = true;
  }
  DrawDockSpace();
  DrawViewport();
}

void ImGuiEditor::Render() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

ImGuiID ImGuiEditor::DockspaceId() {
  return ImGui::GetID("SkelEditDockspace");
}

void ImGuiEditor::CreateDockLayout(int viewport_width, int viewport_height) {
  const ImGuiID dockspace_id = DockspaceId();
  ImGui::DockBuilderRemoveNode(dockspace_id);
  ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
  ImGui::DockBuilderSetNodeSize(dockspace_id,
                                ImVec2(viewport_width, viewport_height));
  ImGui::DockBuilderDockWindow("Viewport", dockspace_id);
  ImGui::DockBuilderFinish(dockspace_id);
}

void ImGuiEditor::DrawDockSpace() {
  ImGui::DockSpaceOverViewport(DockspaceId(), ImGui::GetMainViewport());
}

void ImGuiEditor::SetViewportTextureId(unsigned int viewport_texture_id) {
  viewport_texture_id_ = viewport_texture_id;
}

void ImGuiEditor::DrawViewport() {
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::Begin("Viewport");
  const ImVec2 content_region = ImGui::GetContentRegionAvail();
  const int width = static_cast<int>(content_region.x);
  const int height = static_cast<int>(content_region.y);
  if (width != viewport_width_ || height != viewport_height_) {
    if (viewport_resize_callback_) {
      viewport_resize_callback_(width, height);
    }
    viewport_width_ = width;
    viewport_height_ = height;
  }
  ImGui::Image(static_cast<ImTextureID>(viewport_texture_id_),
               ImVec2(viewport_width_, viewport_height_), ImVec2(0.0f, 1.0f),
               ImVec2(1.0f, 0.0f));
  ImGui::End();
  ImGui::PopStyleVar();
}

}  // namespace skeleton
