// Copyright 2026 aljabari

#include "skeledit/imguieditor.h"

#include <gtest/gtest.h>
#include <imgui.h>
#include <imgui_internal.h>

namespace skeleton {
namespace {

class ImGuiEditorTest : public ::testing::Test {
 protected:
  void SetUp() override {
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.DisplaySize = ImVec2(800.0f, 600.0f);
    io.DeltaTime = 1.0f / 60.0f;
    io.Fonts->Build();
  }

  void TearDown() override { ImGui::DestroyContext(); }

  void DrawViewportWindowOnce() {
    ImGui::Begin("Viewport");
    ImGui::End();
  }

  void DrawLogsWindowOnce() {
    ImGui::Begin("Logs");
    ImGui::End();
  }
};

TEST_F(ImGuiEditorTest, CreateDockLayoutDocksViewportAndLogsWindows) {
  ImGui::NewFrame();
  DrawViewportWindowOnce();
  DrawLogsWindowOnce();

  ImGuiEditor::CreateDockLayout(800, 600);

  ImGuiWindow* viewport = ImGui::FindWindowByName("Viewport");
  ASSERT_NE(viewport, nullptr);
  EXPECT_NE(viewport->DockId, 0);

  ImGuiWindow* logs = ImGui::FindWindowByName("Logs");
  ASSERT_NE(logs, nullptr);
  EXPECT_NE(logs->DockId, 0);

  ImGuiDockNode* node = ImGui::DockBuilderGetNode(ImGuiEditor::DockspaceId());
  ASSERT_NE(node, nullptr);
  EXPECT_TRUE(node->IsDockSpace());
}

TEST_F(ImGuiEditorTest, CreateDockLayoutIsIdempotent) {
  ImGui::NewFrame();
  DrawViewportWindowOnce();
  DrawLogsWindowOnce();

  ImGuiEditor::CreateDockLayout(800, 600);
  ImGuiEditor::CreateDockLayout(800, 600);

  ImGuiWindow* viewport = ImGui::FindWindowByName("Viewport");
  ASSERT_NE(viewport, nullptr);
  EXPECT_NE(viewport->DockId, 0);

  ImGuiWindow* logs = ImGui::FindWindowByName("Logs");
  ASSERT_NE(logs, nullptr);
  EXPECT_NE(logs->DockId, 0);

  ImGuiDockNode* node = ImGui::DockBuilderGetNode(ImGuiEditor::DockspaceId());
  ASSERT_NE(node, nullptr);
  EXPECT_TRUE(node->IsDockSpace());
}

TEST_F(ImGuiEditorTest, WindowsDockIntoDockspaceHost) {
  ImGui::NewFrame();
  ImGuiEditor::CreateDockLayout(800, 600);
  ImGui::DockSpaceOverViewport(ImGuiEditor::DockspaceId(),
                               ImGui::GetMainViewport());
  DrawViewportWindowOnce();
  DrawLogsWindowOnce();
  ImGui::Render();
  ImGui::EndFrame();

  ImGui::NewFrame();
  ImGui::DockSpaceOverViewport(ImGuiEditor::DockspaceId(),
                               ImGui::GetMainViewport());
  DrawViewportWindowOnce();
  DrawLogsWindowOnce();

  ImGuiWindow* viewport = ImGui::FindWindowByName("Viewport");
  ASSERT_NE(viewport, nullptr);
  ASSERT_NE(viewport->DockNode, nullptr);
  ImGuiDockNode* root = viewport->DockNode;
  while (root->ParentNode != nullptr) {
    root = root->ParentNode;
  }
  EXPECT_EQ(root->ID, ImGuiEditor::DockspaceId());

  ImGui::EndFrame();
}

}  // namespace
}  // namespace skeleton
