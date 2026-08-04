# Build System

- **Build tool:** CMake (minimum 3.16)
- **Generator:** Visual Studio (preferred on Windows), Ninja (alternative)
- **C++ standard:** C++20 (required, not optional)
- **Dependencies:** GLFW, Dear ImGui, spdlog (fetched automatically via `FetchContent`)

## Targets

| Target              | Type       | Description                         |
|---------------------|------------|-------------------------------------|
| `libskeleton`       | STATIC     | Core library — all logic lives here |
| `skeleton`          | EXECUTABLE | Main entry point, links `libskeleton` |
| `skeledit`          | EXECUTABLE | Editor entry point, links `libskeleton` and `imgui` |
| `libskeleton_tests` | EXECUTABLE | Unit tests for `libskeleton`, built only when `SKELETON_BUILD_TESTS` is `ON` |
| `skeledit_tests`    | EXECUTABLE | Unit tests for `skeledit`'s `ImGuiEditor`, built only when `SKELETON_BUILD_TESTS` is `ON` |

Each target's `CMakeLists.txt` also calls
[`source_group(TREE ...)`](https://cmake.org/cmake/help/latest/command/source_group.html)
with a `Source Files` prefix, so the directory layout of the sources is
mirrored in the Visual Studio solution explorer. Executable targets (`skeleton`,
`skeledit`) are near-identical duplicates that only set up `main.cc` and link
`libskeleton`.

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

## Renderer factory and fallback priority

`RendererBackend` (in `renderer.h`) lists the renderer backends
(`kVulkan`, `kOpenGl`). Renderers are created through the factory in
`libskeleton/include/libskeleton/rendererfactory.h`:

- `RendererPriorityOrder()` returns the platform's fallback priority order.
  New backends are added there (for example, a future DirectX backend on
  Windows would be listed before Vulkan).
- `CreateRenderer(preferred, render_target)` creates the preferred backend
  first and falls back through the platform priority order when a backend
  cannot be created. A backend is skipped when it has no creator, its creator
  returns `nullptr`, or it throws `RendererCreationException`. The core
  algorithm takes an explicit priority list and creator map so it can be unit
  tested with fake creators.
- `CreateRenderer(render_target)` is the same but has no preferred
  backend: it returns the first backend in the platform priority order that
  can be created.
- The `render_target` argument is a `RenderTarget` enum
  (`RenderTarget::kRenderTargetWindow` — the default — or
  `RenderTarget::kRenderTargetTexture`).
- `VulkanRenderer` initialises itself in its constructor: it creates a
  `VulkanInstance` (volk loader + Vulkan instance) and a `VulkanDevice`
  (physical device with a graphics queue family + logical device). If any step
  fails (loader, instance, physical device, or logical device) it throws
  `RendererCreationException`, so the factory catches it and falls back to the
  next backend. The OpenGL creator always succeeds on supported platforms.
  The private `VulkanInstance`/`VulkanDevice` RAII helpers live under
  `libskeleton/src/renderer/vulkan/`.

Both executables build their renderer through `CreateRenderer()` with no
preferred backend, so they get the first backend in the platform priority
order that can be created (Vulkan once it renders, OpenGL otherwise). `skeledit`
passes `RenderTarget::kRenderTargetTexture` because it needs the texture APIs.
Those APIs live on the base `Renderer` interface (`GetTextureId()`,
`ResizeRenderTarget()`) with default no-op implementations, so both executables
use the renderer polymorphically through `Renderer&` without downcasting.

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
| volk       | https://github.com/zeux/volk.git           | 1.4.350 |
| spdlog     | https://github.com/gabime/spdlog.git       | v1.17.0 |
| googletest | https://github.com/google/googletest.git   | v1.17.0 |
| imgui      | https://github.com/ocornut/imgui.git       | docking |

`glad` requires a Python interpreter with `jinja2` installed; it is only
declared when OpenGL is supported on the target platform. `volk` is only
declared when `SKELETON_TARGET_SUPPORTS_RENDERER_VULKAN` is set; it is the
runtime meta-loader for Vulkan and links no Vulkan library, since function
pointers are loaded at run time via `volkInitialize()`. volk's
`VOLK_PULL_IN_VULKAN` option (ON by default) pulls the Vulkan headers into its
PUBLIC include directories via `find_package(Vulkan)`, so consumers do not need
to locate the headers themselves. `googletest` is only declared when
`SKELETON_BUILD_TESTS` is `ON`.

`spdlog` is a logging dependency of `libskeleton`, linked `PUBLIC` so consumers
(executables and tests) can use it transitively. Its bundled fmt is used, and
its own examples and tests are disabled. All project logging goes through the
`SPDLOG_*` macros (`SPDLOG_DEBUG`/`SPDLOG_INFO`/`SPDLOG_WARN`/`SPDLOG_ERROR`),
which target the spdlog default logger; the renderer factory logs backend
creation, fallback, and failure reasons, and errors are logged before every
thrown `RendererCreationException`.

The macros compile in only messages at or above the `SPDLOG_ACTIVE_LEVEL`
macro. The build sets it per target: `libskeleton`, `skeleton`, and
`libskeleton_tests` use `SPDLOG_LEVEL_TRACE` in Debug configurations and
`SPDLOG_LEVEL_OFF` everywhere else, so **all logging is stripped out of release
builds**. `skeledit` and `skeledit_tests` define `SPDLOG_ACTIVE_LEVEL` as
`SPDLOG_LEVEL_TRACE` unconditionally, so the editor keeps its logs (and its log
window) in release builds.

All log output shares one canonical format defined in
`libskeleton/include/libskeleton/logging.h`:
`kLogPattern` = `[%Y-%m-%d %H:%M:%S.%e] [%l] %v` (plain) and
`kLogPatternConsole` = the same with the level wrapped in `%^...%$` colour
markers. `skeleton` and `skeledit` call `spdlog::set_pattern()` with these, and
`EditorLogSink` formats each entry with a `spdlog::pattern_formatter` built from
`kLogPattern`, so the editor log window shows exactly the same timestamp, level,
and message text as both consoles (the only difference is ANSI colouring).

`skeledit` configures the default logger with two sinks: a `stdout_color_sink_mt`
(console) and an `EditorLogSink` (`skeledit/include/skeledit/editorlogsink.h`).
`EditorLogSink` implements the `LogSink` interface (`skeledit/include/skeledit/logsink.h`)
and is also an spdlog `base_sink`, so every log message is stored in the
`ImGuiEditor` log window as well as printed to the console. It keeps the newest
`kMaxEntries` (500) entries and is thread-safe. The trailing newline the
formatter appends is stripped so each window entry is rendered on its own line.

`imgui` is only a dependency of `skeledit`; it is fetched and added from
`skeledit/CMakeLists.txt`, not from `cmake/dependencies.cmake`. The upstream
repository intentionally ships no `CMakeLists.txt`, so the project provides one
at `cmake/third_party/imgui/CMakeLists.txt`. `skeledit/CMakeLists.txt` populates
`imgui` with the fully-specified details form of `FetchContent_Populate()`
(the single-argument form is deprecated since CMake 3.30), copies the provided
`CMakeLists.txt` into the fetched source tree, and then adds it as an
`EXCLUDE_FROM_ALL` subdirectory. The `imgui` static library target defines the
core sources (`imgui.cpp`, `imgui_demo.cpp`, `imgui_draw.cpp`,
`imgui_tables.cpp`, `imgui_widgets.cpp`) plus the GLFW backend
(`backends/imgui_impl_glfw.cpp`), which is always built. The OpenGL3 backend
(`backends/imgui_impl_opengl3.cpp`) is added when the
`SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL` platform flag is set, and the Vulkan
backend (`backends/imgui_impl_vulkan.cpp`) when
`SKELETON_TARGET_SUPPORTS_RENDERER_VULKAN` is set. The OpenGL3 backend uses
imgui's bundled runtime loader, so it needs no link-time GL dependency. The
Vulkan backend is compiled with `IMGUI_IMPL_VULKAN_USE_VOLK` and links the
`volk` target, so it loads Vulkan function pointers through volk instead of a
Vulkan library. The target exposes the fetched root and `backends/` as `PUBLIC`
include directories and links `glfw` for the GLFW backend.

The `imgui` dependency tracks the `docking` branch (a moving target, currently
`1.92.9b`), which is required for the window-docking functionality in
`skeledit`. That branch exposes the dock builder API (`DockBuilder*` functions
and `ImGuiDockNode` types) through `imgui_internal.h`, which the editor code
includes alongside the public `<imgui.h>`.

All Dear ImGui usage is contained in the `ImGuiEditor` class
(`skeledit/include/skeledit/imguieditor.h`,
`skeledit/src/imguieditor.cc`): context creation, GLFW/OpenGL3 backend
init/shutdown, the per-frame lifecycle (`NewFrame`/`Render`), and drawing the
dockable viewport. `skeledit/src/main.cc` only wires `ImGuiEditor` up to the
`Window` and the renderer (via the factory). `skeledit_tests` (see [TESTING.md](TESTING.md))
compiles `skeledit/src/imguieditor.cc` alongside its test source so the
headless-testable static dock-layout logic can be exercised without a GL
context.

## Adding a new library target

1. Create a subdirectory with `src/` and `include/<target>/` folders.
2. Add a `CMakeLists.txt` defining the target.
3. Add `add_subdirectory(...)` to the root `CMakeLists.txt`.
