// Copyright 2026 aljabari

#ifndef SKELEDIT_IMGUIBACKEND_H_
#define SKELEDIT_IMGUIBACKEND_H_

#include <imgui.h>

namespace skeleton {

// Renders the ImGui UI into the frame of a renderer backend. Each concrete
// implementation pairs the GLFW platform backend with one ImGui renderer
// backend (for example OpenGL3 or Vulkan) and owns the lifecycle of both.
class ImGuiBackend {
 public:
  virtual ~ImGuiBackend() = default;

  // Initialises the platform and renderer backends. Called by the editor after
  // the ImGui context exists. Returns false when the backend cannot be
  // initialised; the caller must not call NewFrame, RenderDrawData, or
  // Shutdown after a failed Init.
  virtual bool Init() = 0;

  // Starts a new ImGui frame. Called before ImGui::NewFrame.
  virtual void NewFrame() = 0;

  // Draws the recorded ImGui draw data into the current frame. Called after
  // ImGui::Render. Backends that draw deferred (for example inside a
  // renderer's own frame recording) may do nothing here.
  virtual void RenderDrawData() = 0;

  // Returns the ImGui texture identifier of the renderer's render target (for
  // example the editor's viewport image), or ImTextureID_Invalid when the
  // renderer does not render to a texture or the identifier is not ready yet.
  // The identifier can change between frames (the render target is recreated,
  // for example on resize), so call it once per frame while drawing.
  virtual ImTextureID GetViewportTextureId() const = 0;

  // Releases the platform and renderer backend resources. Called before the
  // ImGui context is destroyed, and must still run while the renderer backend
  // is alive.
  virtual void Shutdown() = 0;
};

}  // namespace skeleton

#endif  // SKELEDIT_IMGUIBACKEND_H_
