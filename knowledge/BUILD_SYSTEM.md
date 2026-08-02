# Build System

- **Build tool:** CMake (minimum 3.16)
- **Generator:** Visual Studio (preferred on Windows), Ninja (alternative)
- **C++ standard:** C++20 (required, not optional)
- **Dependencies:** GLFW (fetched automatically via `FetchContent`)

## Targets

| Target              | Type       | Description                         |
|---------------------|------------|-------------------------------------|
| `libskeleton`       | STATIC     | Core library — all logic lives here |
| `skeleton`          | EXECUTABLE | Main entry point, links `libskeleton` |
| `libskeleton_tests` | EXECUTABLE | Unit tests, built only when `SKELETON_BUILD_TESTS` is `ON` |

Each target's `CMakeLists.txt` also calls
[`source_group(TREE ...)`](https://cmake.org/cmake/help/latest/command/source_group.html)
with a `Source Files` prefix, so the directory layout of the sources is
mirrored in the Visual Studio solution explorer.

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

`glad` is fetched and generated only when
`SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL` is set. The OpenGL renderer links
the generated `glad` target (OpenGL 3.3 core) to load GL function pointers via
`gladLoadGL`.

## Resources

Runtime resources live under `libskeleton/res/` (e.g. GLSL shaders in
`libskeleton/res/shaders/`). `libskeleton` exports the compile definition

```cmake
SKELETON_RES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/res"
```

(where `CMAKE_CURRENT_SOURCE_DIR` is `libskeleton/`) so both the library and
its consumers can locate resources at run time. `SKELETON_RES_DIR` is defined
only when OpenGL is supported.

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

## How to test

Unit tests use **Google Test** and are built when `SKELETON_BUILD_TESTS` is
`ON` (the default). See [TESTING.md](TESTING.md) for details.

```sh
cmake --build build --config Release --target libskeleton_tests
ctest --test-dir build -C Release --output-on-failure
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
| glad       | https://github.com/Dav1dde/glad.git        | v2.0.6 |
| googletest | https://github.com/google/googletest.git   | v1.17.0 |

`glad` requires a Python interpreter with `jinja2` installed; it is only
declared when OpenGL is supported on the target platform. `googletest` is only
declared when `SKELETON_BUILD_TESTS` is `ON`.

## Adding a new library target

1. Create a subdirectory with `src/` and `include/<target>/` folders.
2. Add a `CMakeLists.txt` defining the target.
3. Add `add_subdirectory(...)` to the root `CMakeLists.txt`.
