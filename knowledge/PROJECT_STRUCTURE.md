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
│   └── src/                  # Implementation files
│       ├── window.cc
│       ├── renderer/
│       │   ├── renderer.cc
│       │   ├── opengl/
│       │   │   └── openglrenderer.cc
│       │   └── vulkan/
│       │       └── vulkanrenderer.cc
├── tests/                    # Unit tests (Google Test), mirrors libskeleton/src
│   ├── CMakeLists.txt
│   └── src/
│       ├── window_test.cc
│       └── renderer/
│           ├── opengl/
│           │   └── openglrenderer_test.cc
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
- Target-level `CMakeLists.txt` handles only that target's sources and
  dependencies.
