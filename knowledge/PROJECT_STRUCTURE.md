# Project Structure

```
skeleton/
├── CMakeLists.txt            # Root — C++20, FetchContent deps, subdirectories
├── AGENTS.md                 # Entry point for future agents
├── cmake/                    # CMake modules
│   ├── CPPLint.cmake         # cpplint integration
│   ├── dependencies.cmake    # FetchContent dependencies
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
│       │   ├── opengl/
│       │   │   ├── openglmesh.cc/.h       # Private: VAO/VBO wrapper
│       │   │   ├── openglrenderer.cc
│       │   │   └── openglshader.cc/.h     # Private: shader program wrapper
│       │   └── vulkan/
│       │       └── vulkanrenderer.cc
├── tests/                    # Unit tests (Google Test), mirrors libskeleton/src
│   ├── CMakeLists.txt
│   └── src/
│       ├── window_test.cc
│       └── renderer/
│           ├── opengl/
│           │   ├── openglmesh_test.cc
│           │   ├── openglrenderer_test.cc
│           │   └── openglshader_test.cc
│           └── vulkan/
│               └── vulkanrenderer_test.cc
├── skeleton/                 # Executable target
│   ├── CMakeLists.txt
│   └── src/
│       └── main.cc
└── build/                    # Build output (gitignored)
```

## Target dependency graph

```
libskeleton_tests ──links──► libskeleton
skeleton          ──links──► libskeleton
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
