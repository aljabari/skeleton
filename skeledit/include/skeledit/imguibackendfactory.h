// Copyright 2026 aljabari

#ifndef SKELEDIT_IMGUIBACKENDFACTORY_H_
#define SKELEDIT_IMGUIBACKENDFACTORY_H_

#include <memory>

#include "libskeleton/renderer.h"

struct GLFWwindow;

namespace skeleton {

class ImGuiBackend;

// Creates the ImGui backend matching |renderer|'s backend, or nullptr when the
// renderer's backend has no ImGui backend. The renderer must be fully
// initialised (CreateContext called) and must outlive the returned backend.
std::unique_ptr<ImGuiBackend> CreateImGuiBackend(GLFWwindow* window,
                                                 Renderer* renderer);

}  // namespace skeleton

#endif  // SKELEDIT_IMGUIBACKENDFACTORY_H_
