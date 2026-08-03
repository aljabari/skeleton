// Copyright 2026 aljabari

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#include "libskeleton/opengl/openglrenderer.h"
#include "libskeleton/window.h"

namespace skeleton {

int Run(int argc, char* argv[]) {
  OpenGlRenderer renderer(true);
  Window window(800, 600, "SkelEdit", renderer);

  ImGui::CreateContext();
  ImGui::GetIO().IniFilename = nullptr;
  ImGui_ImplGlfw_InitForOpenGL(window.GetNativeWindow(), true);
  ImGui_ImplOpenGL3_Init("#version 130");

  bool show_demo_window = true;
  while (window.IsOpen()) {
    window.PollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowDemoWindow(&show_demo_window);

    ImGui::Begin("Viewport");
    ImGui::Image(renderer.GetTextureId(), ImVec2(800.0f, 600.0f),
                 ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
    ImGui::End();

    ImGui::Render();
    renderer.Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    window.SwapBuffers();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  return 0;
}

}  // namespace skeleton

int main(int argc, char* argv[]) {
  return skeleton::Run(argc, argv);
}
