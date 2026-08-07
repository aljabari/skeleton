# Project Structure

```
skeleton/
├── CMakeLists.txt            # Root — C++20, FetchContent deps, subdirectories
├── AGENTS.md                 # Entry point for future agents
├── cmake/                    # CMake modules
│   ├── CPPLint.cmake         # cpplint integration
│   ├── dependencies.cmake    # FetchContent dependencies
│   ├── version.cmake         # skeleton_generate_version(): wires build-time version header generation
│   ├── scripts/
│   │   └── generate_version.cmake # Build-time git-based version calculator (cmake -P)
│   ├── third_party/          # Build recipes for dependencies without their own CMake
│   │   └── imgui/CMakeLists.txt  # Dear ImGui build script, copied into the fetched source
│   └── platform/             # Per-platform capability flags
│       └── windows.cmake     # Defines SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL
├── knowledge/                # Detailed project documentation
│   ├── README.md
│   ├── BUILD_SYSTEM.md
│   ├── CODING_CONVENTIONS.md
│   ├── PROJECT_STRUCTURE.md
│   └── TESTING.md
├── scripts/                  # Build & run scripts
│   └── build_and_run.bat
├── libskeleton/              # Static library target
│   ├── CMakeLists.txt
│   ├── include/libskeleton/  # Public headers
│   │   ├── logging.h
│   │   ├── scene.h           # Scene (ECS, wraps entt::registry) + MeshComponent
│   │   ├── version.h.in      # CMake template expanded to version.h in the build tree on every build
│   │   ├── window.h
│   │   ├── renderer.h
│   │   ├── rendererfactory.h
│   │   ├── opengl/
│   │   │   └── openglrenderer.h
│   │   └── vulkan/
│   │       └── vulkanrenderer.h
│   ├── res/                  # Vulkan-style GLSL shader sources (compiled to SPIR-V at build time)
│   │   └── shaders/
│   │       ├── triangle.frag
│   │       └── triangle.vert
│   └── src/                  # Implementation files (private headers live here too)
│       ├── scene.cc
│       ├── window.cc
│       ├── renderer/
│       │   ├── renderer.cc
│       │   ├── rendererfactory.cc
│       │   ├── opengl/
│       │   │   ├── openglframebuffer.cc/.h    # Private: framebuffer + colour texture wrapper
│       │   │   ├── openglmesh.cc/.h           # Private: VAO/VBO wrapper
│       │   │   ├── openglrenderer.cc
│       │   │   ├── openglshader.cc/.h         # Private: shader program wrapper; cross-compiles SPIR-V to GLSL via SPIRV-Cross
│       │   │   └── opengltexture.cc/.h        # Private: texture wrapper
│       │   └── vulkan/
│       │       ├── vulkancommandbuffer.cc/.h # Private: RAII command pool + primary command buffer
│       │       ├── vulkandevice.cc/.h    # Private: RAII logical device + graphics queue
│       │       ├── vulkanfence.cc/.h     # Private: RAII synchronisation fence
│       │       ├── vulkanframebuffer.cc/.h # Private: RAII framebuffer over a swapchain image view
│       │       ├── vulkangraphicspipeline.cc/.h # Private: RAII graphics pipeline + shader modules; dynamic viewport/scissor so one pipeline draws to the swapchain and the render target
│       │       ├── vulkaninstance.cc/.h  # Private: RAII instance + physical-device enumeration
│       │       ├── vulkanmesh.cc/.h      # Private: RAII vertex buffer + device memory
│       │       ├── vulkanrenderpass.cc/.h # Private: RAII render pass over a colour attachment; final layout parameterised (present-ready for the swapchain, shader-read-only for the render target)
│       │       ├── vulkanrendertarget.cc/.h # Private: RAII off-screen colour image + image view for texture rendering
│       │       ├── vulkansemaphore.cc/.h # Private: RAII synchronisation semaphore
│       │       ├── vulkanswapchain.cc/.h # Private: RAII swapchain + image views
│       │       ├── vulkanvalidation.cc/.h  # Private: RAII debug messenger for validation layers
│       │       └── vulkanrenderer.cc
├── tests/                    # Unit tests (Google Test), mirrors libskeleton/src
│   ├── CMakeLists.txt
│   └── src/
│       ├── scene_test.cc
│       ├── window_test.cc
│       ├── skeledit/
│       │   ├── editorlogsink_test.cc   # EditorLogSink buffering tests (skeledit_tests)
│       │   └── imguieditor_test.cc   # ImGuiEditor dock-layout tests (skeledit_tests)
│       └── renderer/
│           ├── opengl/
│           │   ├── openglmesh_test.cc
│           │   ├── openglrenderer_test.cc
│           │   ├── openglshader_test.cc
│           │   └── opengltexture_test.cc
│           ├── rendererfactory_test.cc
│           └── vulkan/
│               ├── vulkancommandbuffer_test.cc
│               ├── vulkandevice_test.cc
│               ├── vulkanfence_test.cc
│               ├── vulkanframebuffer_test.cc
│               ├── vulkangraphicspipeline_test.cc
│               ├── vulkaninstance_test.cc
│               ├── vulkanmesh_test.cc
│               ├── vulkanrenderer_test.cc
│               ├── vulkanrenderpass_test.cc
│               ├── vulkanrendertarget_test.cc
│               ├── vulkansemaphore_test.cc
│               └── vulkanswapchain_test.cc
├── skeleton/                 # Executable target
│   ├── CMakeLists.txt
│   └── src/
│       └── main.cc
├── skeledit/                 # Editor executable target, links libskeleton and imgui
│   ├── CMakeLists.txt        # Fetches imgui docking branch (skeledit-only dependency)
│   ├── include/skeledit/
│   │   ├── editorlogsink.h   # spdlog sink + LogSink impl for the editor log window
│   │   ├── imguibackend.h    # Abstract ImGuiBackend interface (GLFW platform + renderer backend); GetViewportTextureId supplies the scene preview texture; FlipsViewportTexture reports whether it is bottom-up (OpenGL) and must be flipped when drawn
│   │   ├── imguibackendfactory.h # CreateImGuiBackend: builds the backend for a Renderer
│   │   ├── imguieditor.h     # Public header for the ImGui editor wrapper
│   │   ├── logsink.h         # LogLevel/LogEntry/LogSink interface
│   │   ├── opengl_imguibackend.h # GLFW + ImGui OpenGL3 backend (viewport texture = renderer GetTextureId, stored bottom-up so FlipsViewportTexture returns true)
│   │   ├── vulkan_imguibackend.h # GLFW + ImGui Vulkan backend (own render pass/descriptor pool,
│   │   │                          #   draws via renderer FrameSubmitCallback command buffer hook;
│   │   │                          #   registers the render-target image view via ImGui_ImplVulkan_AddTexture)
│   └── src/
│       ├── main.cc
│       ├── editorlogsink.cc  # Buffers newest kMaxEntries log lines (canonical format)
│       ├── imguibackendfactory.cc # Backend factory dispatching on Renderer::GetBackend
│       ├── imguieditor.cc    # ImGui context/backends, dockable viewport, dock layout (viewport, right-docked scene graph spanning full height, bottom logs), log window; flips the viewport texture only when the backend reports it is stored bottom-up
│       ├── opengl_imguibackend.cc  # ImGui_ImplGlfw_InitForOpenGL + ImGui_ImplOpenGL3_*
│       └── vulkan_imguibackend.cc  # ImGui_ImplGlfw_InitForVulkan + ImGui_ImplVulkan_* + FrameSubmitCallback
└── build/                    # Build output (gitignored)
```

## Target dependency graph

```
libskeleton_tests ──links──► libskeleton
skeledit_tests    ──links──► imgui, glfw
skeleton          ──links──► libskeleton
skeledit          ──links──► libskeleton, imgui
```

## Key conventions

- Each target lives in its own subdirectory.
- Each target has `src/` and `include/<target>/` directories.
- Public headers live under `include/<target>/`.
- Implementation-local headers (classes not exposed to clients) live next to
  their `.cc` files under `src/` and are not part of the public include path.
  Example: `OpenGlShader`/`OpenGlMesh` are declared in
  `libskeleton/src/renderer/opengl/` and only reachable from inside
  `libskeleton` (or tests).
- Target-level `CMakeLists.txt` handles only that target's sources and
  dependencies.
