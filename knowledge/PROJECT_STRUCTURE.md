# Project Structure

```
skeleton/
├── CMakeLists.txt            # Root — C++20, FetchContent deps, subdirectories
├── AGENTS.md                 # Entry point for future agents
├── cmake/                    # CMake modules
│   ├── CPPLint.cmake         # cpplint integration
│   ├── dependencies.cmake    # FetchContent dependencies
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
│   │   ├── window.h
│   │   ├── renderer.h
│   │   ├── rendererfactory.h
│   │   ├── opengl/
│   │   │   └── openglrenderer.h
│   │   └── vulkan/
│   │       └── vulkanrenderer.h
│   ├── res/                  # Runtime resources (shaders)
│   │   └── shaders/
│   │       ├── triangle.frag
│   │       └── triangle.vert
│   └── src/                  # Implementation files (private headers live here too)
│       ├── window.cc
│       ├── renderer/
│       │   ├── renderer.cc
│       │   ├── rendererfactory.cc
│       │   ├── opengl/
│       │   │   ├── openglframebuffer.cc/.h    # Private: framebuffer + colour texture wrapper
│       │   │   ├── openglmesh.cc/.h           # Private: VAO/VBO wrapper
│       │   │   ├── openglrenderer.cc
│       │   │   ├── openglshader.cc/.h         # Private: shader program wrapper
│       │   │   └── opengltexture.cc/.h        # Private: texture wrapper
│       │   └── vulkan/
│       │       ├── vulkandevice.cc/.h    # Private: RAII logical device + graphics queue
│       │       ├── vulkaninstance.cc/.h  # Private: RAII instance + physical-device enumeration
│       │       └── vulkanrenderer.cc
├── tests/                    # Unit tests (Google Test), mirrors libskeleton/src
│   ├── CMakeLists.txt
│   └── src/
│       ├── window_test.cc
│       ├── skeledit/
│       │   └── imguieditor_test.cc   # ImGuiEditor dock-layout tests (skeledit_tests)
│       └── renderer/
│           ├── opengl/
│           │   ├── openglmesh_test.cc
│           │   ├── openglrenderer_test.cc
│           │   ├── openglshader_test.cc
│           │   └── opengltexture_test.cc
│           ├── rendererfactory_test.cc
│           └── vulkan/
│               ├── vulkandevice_test.cc
│               ├── vulkaninstance_test.cc
│               └── vulkanrenderer_test.cc
├── skeleton/                 # Executable target
│   ├── CMakeLists.txt
│   └── src/
│       └── main.cc
├── skeledit/                 # Editor executable target, links libskeleton and imgui
│   ├── CMakeLists.txt        # Fetches imgui docking branch (skeledit-only dependency)
│   ├── include/skeledit/
│   │   └── imguieditor.h     # Public header for the ImGui editor wrapper
│   └── src/
│       ├── main.cc
│       └── imguieditor.cc    # ImGui context/backends, dockable viewport, dock layout
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
