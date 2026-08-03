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
};

TEST_F(ImGuiEditorTest, CreateDockLayoutDocksViewportWindow) {
  ImGui::NewFrame();
  DrawViewportWindowOnce();

  ImGuiEditor::CreateDockLayout(800, 600);

  ImGuiWindow* viewport = ImGui::FindWindowByName("Viewport");
  ASSERT_NE(viewport, nullptr);
  EXPECT_EQ(viewport->DockId, ImGuiEditor::DockspaceId());

  ImGuiDockNode* node = ImGui::DockBuilderGetNode(ImGuiEditor::DockspaceId());
  ASSERT_NE(node, nullptr);
  EXPECT_TRUE(node->IsDockSpace());
}

TEST_F(ImGuiEditorTest, CreateDockLayoutIsIdempotent) {
  ImGui::NewFrame();
  DrawViewportWindowOnce();

  ImGuiEditor::CreateDockLayout(800, 600);
  ImGuiEditor::CreateDockLayout(800, 600);

  ImGuiWindow* viewport = ImGui::FindWindowByName("Viewport");
  ASSERT_NE(viewport, nullptr);
  EXPECT_EQ(viewport->DockId, ImGuiEditor::DockspaceId());
}

TEST_F(ImGuiEditorTest, ViewportDocksIntoDockspaceHost) {
  ImGui::NewFrame();
  ImGuiEditor::CreateDockLayout(800, 600);
  ImGui::DockSpaceOverViewport(ImGuiEditor::DockspaceId(),
                               ImGui::GetMainViewport());
  DrawViewportWindowOnce();
  ImGui::Render();
  ImGui::EndFrame();

  ImGui::NewFrame();
  ImGui::DockSpaceOverViewport(ImGuiEditor::DockspaceId(),
                               ImGui::GetMainViewport());
  DrawViewportWindowOnce();

  ImGuiWindow* viewport = ImGui::FindWindowByName("Viewport");
  ASSERT_NE(viewport, nullptr);
  ASSERT_NE(viewport->DockNode, nullptr);
  EXPECT_EQ(viewport->DockNode->ID, ImGuiEditor::DockspaceId());

  ImGui::EndFrame();
}

}  // namespace
}  // namespace skeleton
