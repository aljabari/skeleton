// Copyright 2026 aljabari

#include "skeledit/opengl_imguibackend.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

namespace skeleton {

OpenGlImGuiBackend::OpenGlImGuiBackend(GLFWwindow* window) : window_(window) {}

OpenGlImGuiBackend::~OpenGlImGuiBackend() {
  if (initialised_) {
    Shutdown();
  }
}

bool OpenGlImGuiBackend::Init() {
  if (!ImGui_ImplGlfw_InitForOpenGL(window_, true)) {
    return false;
  }
  if (!ImGui_ImplOpenGL3_Init("#version 130")) {
    ImGui_ImplGlfw_Shutdown();
    return false;
  }
  initialised_ = true;
  return true;
}

void OpenGlImGuiBackend::NewFrame() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
}

void OpenGlImGuiBackend::RenderDrawData() {
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void OpenGlImGuiBackend::Shutdown() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  initialised_ = false;
}

}  // namespace skeleton
