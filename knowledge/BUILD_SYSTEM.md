# Build System

- **Build tool:** CMake (minimum 3.16)
- **Generator:** Visual Studio (preferred on Windows), Ninja (alternative)
- **C++ standard:** C++20 (required, not optional)
- **Dependencies:** GLFW (fetched automatically via `FetchContent`)

## Targets

| Target       | Type   | Description                         |
|--------------|--------|-------------------------------------|
| `libskeleton`| STATIC | Core library — all logic lives here |
| `skeleton`   | EXECUTABLE | Main entry point, links `libskeleton` |

## Platform support

`cmake/platform/windows.cmake` is included at the top of the root
`CMakeLists.txt` when `WIN32` is true and defines capability flags for the
target platform:

| Flag                                       | Meaning                          |
|--------------------------------------------|----------------------------------|
| `SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL` | Target platform supports OpenGL  |
| `SKELETON_TARGET_SUPPORTS_RENDERER_VULKAN` | Target platform supports Vulkan  |

The renderer sources in `libskeleton` are only added to the build when the
corresponding `SKELETON_TARGET_SUPPORTS_RENDERER_*` flag is defined. In that
case `libskeleton` also exports the same name as a `PUBLIC` compile definition,
so client code can guard against the API with:

```cpp
#ifdef SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL
#include "libskeleton/opengl/openglrenderer.h"
#endif

#ifdef SKELETON_TARGET_SUPPORTS_RENDERER_VULKAN
#include "libskeleton/vulkan/vulkanrenderer.h"
#endif
```

## How to build

```sh
cmake -B build -G "Visual Studio 18"
cmake --build build --config Release
```

**cpplint** runs automatically before each target is built. If linting finds
any issues, the build fails. You can also run linting standalone:

```sh
cmake --build build --target libskeleton_cpplint
cmake --build build --target skeleton_cpplint
```

or all at once:

```sh
cmake --build build --target skeleton  # triggers both targets
```

## How to open in Visual Studio

```sh
scripts\windows\build_projects.bat
```

This configures the project (if needed) and opens `skeleton.slnx`.

Once inside Visual Studio, build and run via `F5` or `Ctrl+F5`. The executable
will be at `build\skeleton\Release\skeleton.exe` (or `Debug` depending on
configuration).

## Dependencies

The project uses **CMake FetchContent** to download and build dependencies:

| Dependency | Source                                     | Tag |
|------------|--------------------------------------------|-----|
| GLFW       | https://github.com/glfw/glfw.git           | 3.4 |

## Adding a new library target

1. Create a subdirectory with `src/` and `include/<target>/` folders.
2. Add a `CMakeLists.txt` defining the target.
3. Add `add_subdirectory(...)` to the root `CMakeLists.txt`.
