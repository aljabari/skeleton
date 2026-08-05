// Copyright 2026 aljabari

#include "skeledit/imguieditor.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#include "skeledit/imguibackend.h"

namespace skeleton {

namespace {

// Colour used to render a log message of the given level.
ImVec4 LogLevelColor(LogLevel level) {
  switch (level) {
    case LogLevel::kDebug:
      return ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
    case LogLevel::kInfo:
      return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    case LogLevel::kWarn:
      return ImVec4(1.0f, 0.85f, 0.4f, 1.0f);
    case LogLevel::kError:
      return ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
    case LogLevel::kCritical:
      return ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
  }
  return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
}

}  // namespace

ImGuiEditor::ImGuiEditor(
    std::unique_ptr<ImGuiBackend> backend, int viewport_width,
    int viewport_height, std::shared_ptr<LogSink> log_sink,
    std::function<void(int, int)> viewport_resize_callback)
    : backend_(std::move(backend)),
      viewport_width_(viewport_width),
      viewport_height_(viewport_height),
      log_sink_(std::move(log_sink)),
      viewport_resize_callback_(std::move(viewport_resize_callback)) {
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.IniFilename = nullptr;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  if (backend_ == nullptr || !backend_->Init()) {
    ImGui::DestroyContext();
    throw std::runtime_error("Failed to initialise the ImGui backend.");
  }
}

ImGuiEditor::~ImGuiEditor() {
  if (backend_ != nullptr) {
    backend_->Shutdown();
  }
  ImGui::DestroyContext();
}

void ImGuiEditor::NewFrame() {
  backend_->NewFrame();
  ImGui::NewFrame();
}

void ImGuiEditor::Draw() {
  if (!dock_layout_initialised_) {
    CreateDockLayout(viewport_width_, viewport_height_);
    dock_layout_initialised_ = true;
  }
  DrawDockSpace();
  DrawViewport();
  DrawLogs();
}

void ImGuiEditor::Render() {
  ImGui::Render();
  backend_->RenderDrawData();
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
  ImGuiID viewport_node = dockspace_id;
  const ImGuiID logs_node = ImGui::DockBuilderSplitNode(
      viewport_node, ImGuiDir_Down, 0.25f, nullptr, &viewport_node);
  ImGui::DockBuilderDockWindow("Viewport", viewport_node);
  ImGui::DockBuilderDockWindow("Logs", logs_node);
  ImGui::DockBuilderFinish(dockspace_id);
}

void ImGuiEditor::DrawDockSpace() {
  ImGui::DockSpaceOverViewport(DockspaceId(), ImGui::GetMainViewport());
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
  // Query the texture after the resize callback so a render target the callback
  // just recreated (for example the Vulkan renderer recreating its image) is
  // already reflected in the identifier the backend returns.
  const ImTextureID viewport_texture_id = backend_->GetViewportTextureId();
  if (viewport_texture_id != 0) {
    ImGui::Image(viewport_texture_id,
                 ImVec2(viewport_width_, viewport_height_), ImVec2(0.0f, 1.0f),
                 ImVec2(1.0f, 0.0f));
  } else {
    // The backend has no render target texture yet (for example the renderer
    // does not render to a texture).
    ImGui::TextUnformatted("No render target.");
  }
  ImGui::End();
  ImGui::PopStyleVar();
}

void ImGuiEditor::DrawLogs() {
  if (log_sink_ == nullptr) {
    return;
  }
  ImGui::Begin("Logs");
  if (ImGui::Button("Clear")) {
    log_sink_->Clear();
  }
  ImGui::SameLine();
  ImGui::TextUnformatted("Auto-scrolls to bottom");
  ImGui::Separator();

  ImGui::BeginChild("LogScrollingRegion", ImVec2(0.0f, 0.0f), false,
                    ImGuiWindowFlags_HorizontalScrollbar);
  const std::vector<LogEntry> entries = log_sink_->Entries();
  for (const LogEntry& entry : entries) {
    ImGui::TextColored(LogLevelColor(entry.level), "%s",
                       entry.message.c_str());
  }
  if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
    ImGui::SetScrollHereY(1.0f);
  }
  ImGui::EndChild();

  ImGui::End();
}

}  // namespace skeleton
