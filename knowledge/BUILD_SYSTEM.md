# Build System

- **Build tool:** CMake (minimum 3.16)
- **Generator:** Visual Studio (preferred on Windows), Ninja (alternative)
- **C++ standard:** C++20 (required, not optional)
- **Dependencies:** GLFW, Dear ImGui, EnTT, spdlog, SPIRV-Cross (fetched automatically via `FetchContent`)

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

On Windows, the `skeleton` and `skeledit` executables set `WIN32_EXECUTABLE` to
`ON` in Release configurations, so release builds run as GUI applications with
no console window. Debug (and MinSizeRel/RelWithDebInfo) keep the console so
log output stays visible while developing. Because the GUI subsystem makes MSVC
expect a `WinMain` entry point, both targets add `/ENTRY:mainCRTStartup` in
Release so the CRT still calls the app's `main()`.

## Versioning

The version is derived from the git history at build time. `cmake/version.cmake`
(included from the root `CMakeLists.txt`) defines
`skeleton_generate_version()`, which `libskeleton/CMakeLists.txt` calls after
`add_library(libskeleton)`. It registers a `libskeleton_version` custom target
(depended on by `libskeleton` and part of the default build) that runs
`cmake/scripts/generate_version.cmake` via `cmake -P` **on every build**. The
script expands the template `libskeleton/include/libskeleton/version.h.in` into
`include/libskeleton/version.h` in the build tree, and that generated directory
is exported as a `PUBLIC` include directory so `libskeleton` and every consumer
(`skeleton`, `skeledit`, the tests) can `#include "libskeleton/version.h"`.

Version derivation from the repo's commit history:

- **Major/minor** come from the most recent tag reachable from `HEAD` that
  matches `v<major>.<minor>` (via `git describe --tags --match v[0-9]*.[0-9]*`,
  validated against `^v([0-9]+)\\.([0-9]+)$`).
- **Patch** is the number of commits made since that tag
  (`git rev-list --count <tag>..HEAD`).
- If no matching tag exists yet, the version is `0.<commit count>`.
- If git is unavailable (e.g. a source archive without `.git`), the root
  project version from `project(skeleton VERSION 1.0.0 ...)` is used.

The header exposes `SKELETON_VERSION_MAJOR`, `SKELETON_VERSION_MINOR`, and
`SKELETON_VERSION_PATCH` as integer preprocessor defines plus
`SKELETON_VERSION_STRING` as `"v<major>.<minor>.<patch>"`. The script rewrites
the header only when its content actually changed, so an unchanged version does
not trigger recompilation of translation units that include it. Both
executables log the version on startup (`SPDLOG_INFO("Skeleton {} started.",
SKELETON_VERSION_STRING)` and the same for `skeledit`) and include it in the
window title (e.g. `"Skeleton v0.0.63"`). The Vulkan renderer also passes
`VK_MAKE_VERSION(SKELETON_VERSION_MAJOR, SKELETON_VERSION_MINOR,
SKELETON_VERSION_PATCH)` to `VkApplicationInfo`'s `engineVersion` when creating
the instance.

## Renderer factory and fallback priority

`RendererBackend` (in `renderer.h`) lists the renderer backends
(`kVulkan`, `kOpenGl`). Renderers are created through the factory in
`libskeleton/include/libskeleton/rendererfactory.h`:

- `RendererPriorityOrder()` returns the platform's fallback priority order.
  New backends are added there (for example, a future DirectX backend on
  Windows would be listed before Vulkan).
- `CreateRenderer(preferred, render_target, window_config)` creates the
  preferred backend first and falls back through the platform priority order
  when a backend cannot be created. A backend is skipped when it has no
  creator, its creator returns `nullptr`, or its creator or the renderer's
  `CreateContext` throws `RendererCreationException`. The core algorithm takes
  an explicit priority list and creator map so it can be unit tested with fake
  creators.
- `CreateRenderer(render_target, window_config)` is the same but has no
  preferred backend: it returns the first backend in the platform priority
  order that can be created.
- The `render_target` argument is a `RenderTarget` enum
  (`RenderTarget::kRenderTargetWindow` — the default — or
  `RenderTarget::kRenderTargetTexture`).
- The `window_config` argument is a `WindowConfig` struct (in `renderer.h`)
  holding the width, height, and title of the window the renderer creates.
- Window creation is part of context initialisation: the factory calls each
  backend's `CreateContext(window_config)` inside its fallback handling, so a
  backend that fails to create a window or its context (GLFW initialisation,
  Vulkan instance/surface/device, GL context) throws
  `RendererCreationException`, which is caught and falls back to the next
  backend. Renderers own the window they create and destroy it on destruction.
- `VulkanRenderer::CreateContext` initialises GLFW, creates a `VulkanInstance`
  (volk loader + Vulkan instance) with the surface extensions returned by
  `glfwGetRequiredInstanceExtensions`, creates the window with
  `GLFW_CLIENT_API = GLFW_NO_API`, then retrieves the window's surface with
  `glfwCreateWindowSurface` and creates a `VulkanDevice` with that surface: it
  picks a physical device with a graphics queue family and a present queue
  family (the same family when possible) and creates a logical device with a
  graphics queue and a present queue. For texture targets it then creates the
  off-screen `VulkanRenderTarget`, its shader-read-only render pass, and its
  framebuffer. The destructor waits for the device to go idle and destroys the
  render-target/swapchain framebuffers before their images, avoiding
  in-flight-resource destruction.
- `OpenGlRenderer::CreateContext` requests a 3.3 context, creates the window,
  makes its context current, loads the GL functions with glad, and creates the
  shader and (for texture targets) the render target. It no longer creates a
  mesh: geometry now comes from the scene at render time.
- The private `VulkanInstance`/`VulkanDevice`/`VulkanSwapchain`/
  `VulkanGraphicsPipeline`/`VulkanMesh`/`VulkanRenderPass`/
  `VulkanRenderTarget`/`VulkanFramebuffer`/`VulkanCommandBuffer`/`VulkanSemaphore`/`VulkanFence`
  RAII helpers live under `libskeleton/src/renderer/vulkan/`.
  `VulkanRenderTarget` is the off-screen colour image + image view the scene
  draws into in texture mode (created only when the render target is a texture,
  sized from the window config); `VulkanRenderPass` takes a final image layout,
  so the swapchain pass ends in `VK_IMAGE_LAYOUT_PRESENT_SRC_KHR` and the render
  target pass in `VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL` for ImGui sampling.
  `VulkanGraphicsPipeline` uses dynamic viewport/scissor state, and the renderer
  sets them from each target's extent before drawing, so the same pipeline
  renders into the swapchain and the off-screen render target.

Vulkan validation layers are enabled only in Debug builds. In that
configuration `libskeleton` defines `SKELETON_VULKAN_ENABLE_VALIDATION=1`
(exported `PUBLIC`, so tests can guard against it); everywhere else it is `0`.
When the macro is `1` and the `VK_LAYER_KHRONOS_validation` layer is installed
(it ships with the Vulkan SDK), `VulkanInstance` enables that layer and the
`VK_EXT_debug_utils` extension when creating the instance, then creates a
`VulkanValidation` (a private RAII wrapper in `vulkanvalidation.cc/.h` around
the `VkDebugUtilsMessengerEXT`). The messenger callback routes each validation
message into spdlog, mapping severity to logger level (error, warning, info,
and verbose map to `SPDLOG_ERROR`, `SPDLOG_WARN`, `SPDLOG_INFO`, and
`SPDLOG_DEBUG`), so validation output appears in the console and the `skeledit`
log window alongside normal logs. If the layer is unavailable the instance is
still created (with a warning); the messenger and the layer code are compiled
out of non-Debug builds.

Both executables build their renderer through `CreateRenderer(render_target,
window_config)` with no preferred backend, so they get the first backend in
the platform priority order that can be created (Vulkan once it renders, OpenGL
otherwise). The window is created as part of that call, so the application
wraps the renderer's window in a `Window` (a thin facade over the GLFW loop
operations: `IsOpen`, `PollEvents`, `SwapBuffers`, `Maximize`,
`GetNativeWindow`). `skeledit` passes
`RenderTarget::kRenderTargetTexture` because it needs the texture APIs.
Those APIs live on the base `Renderer` interface (`GetTextureId()`,
`ResizeRenderTarget()`) with default no-op implementations, so both executables
use the renderer polymorphically through `Renderer&` without downcasting.
Both backends implement texture mode: OpenGL via `OpenGlFramebuffer`, Vulkan via
the off-screen `VulkanRenderTarget` (whose image view ImGui samples).

## Scene

`Scene` (`libskeleton/include/libskeleton/scene.h`) is the ECS wrapper around an
`entt::registry` (linked through the public `EnTT` target). The registry is
accessed through `Scene::Registry()`. Drawable geometry is an entity carrying a
`MeshComponent`, which holds the mesh's vertices as interleaved position (vec3)
+ colour (vec3) floats authored in the Vulkan coordinate system (the same layout
the shaders expect). Each frame the applications pass their scene to
`Renderer::Render(const Scene&)`, which iterates
`scene.Registry().view<MeshComponent>()` and draws every mesh entity. Both
renderers cache one GPU mesh per scene `MeshComponent` (`OpenGlMesh`/`VulkanMesh`
per entity), rebuilt only when a component's vertices change; Vulkan rebuilds
safely after the in-flight fence wait, so the previous frame's buffers can be
destroyed. `skeleton` and `skeledit` each build a demo scene with a single
triangle mesh entity and draw it every frame.

## Resources

Runtime shader sources live under `libskeleton/res/shaders/`
(`triangle.vert`/`triangle.frag`). They are written in **Vulkan-style GLSL**
(`#version 450` with explicit `layout(location = ...)` qualifiers) because the
Vulkan coordinate system is the single coordinate system for all rendering
backends: the Vulkan renderer consumes the compiled SPIR-V directly, and the
OpenGL renderer cross-compiles the same SPIR-V to desktop GLSL at run time.

### Shader compilation to SPIR-V

When either `SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL` or
`SKELETON_TARGET_SUPPORTS_RENDERER_VULKAN` is set, `libskeleton` compiles the
GLSL shaders under `libskeleton/res/shaders/` to SPIR-V at build time with
**glslc** (searched for via `find_program`; installing the Vulkan SDK or adding
`glslc` to `PATH` is required). The `.spv` files are generated into the build
tree at `${CMAKE_CURRENT_BINARY_DIR}/shaders/` by the `libskeleton_shaders`
custom target, which `libskeleton` depends on so the files exist before the
library (or anything linking it, such as the tests) builds. The shaders use
explicit input/output locations, so no `-fauto-map-locations` flag is needed.

`libskeleton` exports the `PUBLIC` compile definition

```cmake
SKELETON_SHADER_DIR="${CMAKE_CURRENT_BINARY_DIR}/shaders"
```

so the library and its consumers (executables and tests) can locate the
compiled SPIR-V at run time. `VulkanGraphicsPipeline`
(`libskeleton/src/renderer/vulkan/vulkangraphicspipeline.cc/.h`) reads those
files, creates a `VkShaderModule` per stage, and builds the graphics pipeline
from them.

### OpenGL shader cross-compilation

The OpenGL renderer loads the same SPIR-V modules and cross-compiles them to
desktop GLSL at run time with **SPIRV-Cross** (`spirv_cross::CompilerGLSL`,
`spirv-cross-glsl` target, linked `PUBLIC` when OpenGL is supported).
`OpenGlShader` (`libskeleton/src/renderer/opengl/openglshader.cc/.h`) targets
GLSL 3.30 (desktop, not ES) and sets two vertex-stage options so the
Vulkan-authored shaders render correctly in OpenGL's y-up framebuffer:
`flip_vert_y` (emits `gl_Position.y = -gl_Position.y;`) and
`fixup_clipspace` (converts the `[0, w]` clip depth to OpenGL's `[-w, w]`,
emitting `gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;`). The
cross-compiled sources are then compiled and linked exactly like the previous
hand-written GLSL. A `RendererCreationException` is thrown if a SPIR-V file
cannot be read or cross-compiled.

Because the mesh is authored once in the Vulkan coordinate system, both
renderers use the same winding (front faces wind counter-clockwise in the
y-down framebuffer): `VulkanGraphicsPipeline` uses
`VK_FRONT_FACE_COUNTER_CLOCKWISE`, and the y flip applied by the OpenGL vertex
shader makes the same vertices face the OpenGL camera.

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
| entt       | https://github.com/skypjack/entt.git       | v3.16.0 |
| imgui      | https://github.com/ocornut/imgui.git       | docking |
| spirv-cross | https://github.com/KhronosGroup/SPIRV-Cross.git | vulkan-sdk-1.4.350.1 |

`glad` requires a Python interpreter with `jinja2` installed; it is only
declared when OpenGL is supported on the target platform. `volk` is only
declared when `SKELETON_TARGET_SUPPORTS_RENDERER_VULKAN` is set; it is the
runtime meta-loader for Vulkan and links no Vulkan library, since function
pointers are loaded at run time via `volkInitialize()`. volk's
`VOLK_PULL_IN_VULKAN` option (ON by default) pulls the Vulkan headers into its
PUBLIC include directories via `find_package(Vulkan)`, so consumers do not need
to locate the headers themselves. `googletest` is only declared when
`SKELETON_BUILD_TESTS` is `ON`. `spirv-cross` is only declared when OpenGL is
supported; it has no release tags, so it tracks the `vulkan-sdk-1.4.350.1`
tag, and the CLI, tests, and the MSL/HLSL/CPP/REFLECT/C-API/util subprojects
are disabled so only the GLSL backend (`spirv-cross-glsl`, which links
`spirv-cross-core`) is built.

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

Because `libskeleton` is a static library compiled once, its `SPDLOG_ACTIVE_LEVEL`
cannot differ per consumer: in a plain release build only `skeledit`'s own
messages (e.g. "SkelEdit started.") reach the log window, since every log call
inside `libskeleton` is compiled out. To also see library logs (renderer
creation, window creation, Vulkan/GL messages) in a release `skeledit`,
configure with `-DSKELETON_ENABLE_LOGGING_IN_RELEASE=ON`. This compiles
`libskeleton`, `skeleton`, and `libskeleton_tests` with `SPDLOG_LEVEL_TRACE` in
non-Debug configurations too. It is `OFF` by default, keeping release logging
stripped for the standalone `skeleton` console application.

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

All Dear ImGui usage is contained in `skeledit`; `libskeleton` never mentions
ImGui. The ImGui backend seam is the GLFW init call
(`ImGui_ImplGlfw_InitForOpenGL` vs `ImGui_ImplGlfw_InitForVulkan`), so the
editor is split into a backend-neutral shell and per-backend implementations:

- `ImGuiBackend` (`skeledit/include/skeledit/imguibackend.h`) is the abstract
  interface: `Init`, `NewFrame`, `RenderDrawData`, `GetViewportTextureId`,
  `FlipsViewportTexture`, `Shutdown`. `FlipsViewportTexture` returns true when
  the viewport texture is stored bottom-up (OpenGL) and the editor must flip it
  vertically when drawing; Vulkan returns false. Each concrete backend owns the
  paired GLFW + renderer ImGui backends.
- `OpenGlImGuiBackend` (`opengl_imguibackend.cc/.h`) wraps
  `ImGui_ImplGlfw_InitForOpenGL` + `ImGui_ImplOpenGL3_*` and draws immediately
  from `RenderDrawData`.
- `VulkanImGuiBackend` (`vulkan_imguibackend.cc/.h`) wraps
  `ImGui_ImplGlfw_InitForVulkan` + `ImGui_ImplVulkan_*`. `Init` creates the
  Vulkan objects the ImGui backend needs that the renderer does not provide — a
  descriptor pool, a render pass that loads the swapchain image left by the
  renderer's scene render pass (`loadOp` `LOAD`, `initialLayout`/`finalLayout`
  `VK_IMAGE_LAYOUT_PRESENT_SRC_KHR`), one framebuffer per swapchain image, and a
  command pool + command buffer — fills `ImGui_ImplVulkan_InitInfo` from
  `VulkanRenderer` (instance, physical device, device, graphics queue
  family/queue, the backend's own descriptor pool and render pass, and swapchain
  image counts), and registers a frame submit callback on the renderer. Each
  recorded frame the renderer invokes that callback, which resets and records
  the backend's command buffer (its render pass over the current swapchain
  image, `ImGui_ImplVulkan_RenderDrawData`, `vkCmdEndRenderPass`) and returns
  it; the renderer submits it after its own scene command buffer and before
  presenting. `RenderDrawData` is a no-op because the drawing is deferred.
  `Shutdown` drops the callback, waits for the device to go idle, frees the
  viewport texture's descriptor set from the pool (before `ImGui_ImplVulkan_Shutdown`,
  which deletes the backend data `ImGui_ImplVulkan_RemoveTexture` depends on),
  shuts down the ImGui backends, and destroys the backend-owned Vulkan objects.
  The backend also
  owns the viewport texture: `GetViewportTextureId` lazily registers the
  renderer's `RenderTargetImageView()` with
  `ImGui_ImplVulkan_AddTexture` (which on the ImGui docking branch takes an image
  view + `VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL` and returns a
  `VkDescriptorSet` used as `ImTextureID`) and re-registers it when the renderer
  recreates the render target (resize), removing the old set.
- `ImGuiEditor` (`imguieditor.cc/.h`) now takes a `std::unique_ptr<ImGuiBackend>`
  and a non-owning `Scene*` in its constructor, calls `Init` after creating the
  ImGui context, and dispatches `NewFrame`/`Render` through the backend. Each
  frame the viewport draws the backend-provided texture id
  (`GetViewportTextureId`, an `ImTextureID`/`ImU64`) via `ImGui::Image`, or "No
  render target." when there is none; the id is no longer pushed from `main.cc`.
  The UVs passed to `ImGui::Image` flip the texture vertically only when
  `FlipsViewportTexture()` returns true (OpenGL's bottom-up textures), while
  Vulkan's top-down coordinates draw it upright without flipping. The scene
  graph panel lists every entity as an ImGui tree node
  (`registry.view<MeshComponent>()`, labelled "Entity <id>" via
  `entt::to_integral`), with each mesh entity's component shown as a leaf (e.g.
  its vertex count); it falls back to "No scene loaded." when the scene pointer
  is null.
- `CreateImGuiBackend` (`imguibackendfactory.h`, `imguibackendfactory.cc`)
  builds the right backend from `Renderer::GetBackend()`; `skeledit/src/main.cc`
  passes it to the editor. For Vulkan the frame loop calls `editor.Render()`
  *before* `renderer->Render(scene)` so the ImGui draw data exists when the
  renderer records its frame.

The Vulkan renderer supports compositing an extra layer through a generic,
ImGui-agnostic hook: `VulkanRenderer::SetFrameSubmitCallback` accepts a
`std::function<VkCommandBuffer(uint32_t image_index)>` invoked once per recorded
frame after the renderer's own command buffer is recorded (it is skipped for
frames not recorded, e.g. when the swapchain is out of date). The callback
records and returns an additional command buffer — for example the ImGui
backend's, drawn through its own render pass over the acquired swapchain image —
and the renderer submits it together with its own scene command buffer (in
order) before presenting. Returning `VK_NULL_HANDLE` submits nothing extra. To
drive `ImGui_ImplVulkan_InitInfo` and the backend's framebuffers, `VulkanRenderer`
also exposes `RenderPass()`, `QueueFamilyIndex()`, `SwapchainImageCount()`,
`SwapchainMinImageCount()`, `SwapchainImageFormat()`, `SwapchainExtent()`, and
`SwapchainImageViews()` (the swapchain data comes from `VulkanSwapchain`, which
records its requested `minImageCount`), plus `RenderTargetImageView()` and
`RenderTargetExtent()` for the off-screen render target. In texture mode `Render`
records the scene pass into the off-screen render target (so `ImGui::Image` in
the viewport samples it) and also clears the swapchain image through the same
render pass, giving the UI drawn by the frame submit hook a defined background.
`skeledit` additionally defines
`IMGUI_IMPL_VULKAN_USE_VOLK` for its own compilation units so
`imgui_impl_vulkan.h` resolves through volk, matching how the `imgui` library
itself is compiled.

`skeledit_tests` (see [TESTING.md](TESTING.md))
compiles `skeledit/src/imguieditor.cc` alongside its test source so the
headless-testable static dock-layout logic can be exercised without a GL
context. It links `libskeleton` (and through it glfw/spdlog/EnTT) because the
editor now reads the scene graph from a `Scene`.

## Adding a new library target

1. Create a subdirectory with `src/` and `include/<target>/` folders.
2. Add a `CMakeLists.txt` defining the target.
3. Add `add_subdirectory(...)` to the root `CMakeLists.txt`.
