# Project Structure

```
skeleton/
├── CMakeLists.txt            # Root — C++20, FetchContent deps, subdirectories
├── AGENTS.md                 # Entry point for future agents
├── cmake/                    # CMake modules
│   └── CPPLint.cmake         # cpplint integration
├── knowledge/                # Detailed project documentation
│   ├── README.md
│   ├── BUILD_SYSTEM.md
│   ├── CODING_CONVENTIONS.md
│   └── PROJECT_STRUCTURE.md
├── scripts/                  # Build & run scripts
│   └── build_and_run.bat
├── libskeleton/              # Static library target
│   ├── CMakeLists.txt
│   ├── include/libskeleton/  # Public headers
│   │   └── window.h
│   └── src/                  # Implementation files
│       └── window.cpp
├── skeleton/                 # Executable target
│   ├── CMakeLists.txt
│   └── src/
│       └── main.cpp
└── build/                    # Build output (gitignored)
```

## Target dependency graph

```
skeleton ──links──► libskeleton
```

## Key conventions

- Each target lives in its own subdirectory.
- Each target has `src/` and `include/<target>/` directories.
- Public headers live under `include/<target>/`.
- Target-level `CMakeLists.txt` handles only that target's sources and
  dependencies.
