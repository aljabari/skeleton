# Testing

This project uses **Google Test** (with **Google Mock**) for unit tests.

## Framework

- **Google Test** — test framework (`gtest`)
- **Google Mock** — mocking framework (`gmock`), bundled with Google Test

## Build gate

Tests are built only when the `SKELETON_BUILD_TESTS` CMake option is enabled:

```sh
cmake -B build -DSKELETON_BUILD_TESTS=ON
```

The option **defaults to `ON`**. Set it to `OFF` to compile without tests:

```sh
cmake -B build -DSKELETON_BUILD_TESTS=OFF
```

## Layout

Tests live in the `tests/` directory and mirror the `libskeleton/src/` layout:
each implementation file has a matching `*_test.cc` in the same relative
location. Currently:

| Target              | Sources                                        | Covers                                           |
|---------------------|------------------------------------------------|--------------------------------------------------|
| `libskeleton_tests` | `tests/src/window_test.cc`                     | `Window` as a thin facade over the renderer's window (`IsOpen`, `Maximize`, `GetNativeWindow`, `PollEvents`), verified with a mock renderer |
|                     | `tests/src/renderer/opengl/openglframebuffer_test.cc` | `OpenGlFramebuffer` allocation, binding, resizing, and cleanup |
|                     | `tests/src/renderer/opengl/openglrenderer_test.cc` | `OpenGlRenderer::CreateContext` window/context creation (3.3 context, current context, glad), triangle rendering, render-target resizing, and viewport configuration |
|                     | `tests/src/renderer/opengl/openglshader_test.cc` | `OpenGlShader` compilation/linking and program binding |
|                     | `tests/src/renderer/opengl/openglmesh_test.cc` | `OpenGlMesh` VAO binding and error-free drawing |
|                     | `tests/src/renderer/opengl/opengltexture_test.cc` | `OpenGlTexture` allocation, binding, and cleanup |
|                     | `tests/src/renderer/rendererfactory_test.cc` | `RendererBackend` enum, platform priority order, and fallback creation (`CreateRenderer` with a preferred backend) with injectable fake creators, including creators that return `nullptr` or throw `RendererCreationException`, and renderers whose `CreateContext` throws |
|                     | `tests/src/renderer/vulkan/vulkaninstance_test.cc` | `VulkanInstance` instance creation and physical-device enumeration (skips when Vulkan is unavailable); in Debug builds, debug-messenger creation for validation layers |
|                     | `tests/src/renderer/vulkan/vulkandevice_test.cc` | `VulkanDevice` logical-device/queue creation from a `VulkanInstance` and window surface, including the present queue (skips when Vulkan is unavailable) |
|                     | `tests/src/renderer/vulkan/vulkangraphicspipeline_test.cc` | `VulkanGraphicsPipeline` pipeline/layout creation from the build-time compiled `triangle.vert.spv`/`triangle.frag.spv` shader modules against a render pass and swapchain (skips when Vulkan is unavailable) |
|                     | `tests/src/renderer/vulkan/vulkanrenderer_test.cc` | `VulkanRenderer::CreateContext` instance/surface/window/physical-device/logical-device creation with graphics and present queues (skips when Vulkan is unavailable) |
|                     | `tests/src/renderer/vulkan/vulkanswapchain_test.cc` | `VulkanSwapchain` swapchain and image-view creation, including swapchain extent and format selection (skips when Vulkan is unavailable) |
| `skeledit_tests`    | `tests/src/skeledit/editorlogsink_test.cc` | `EditorLogSink` buffering, canonical timestamped format (`[YYYY-MM-DD HH:MM:SS.mmm] [level] message`), `Clear`, and `kMaxEntries` bound |
|                     | `tests/src/skeledit/imguieditor_test.cc`       | `ImGuiEditor` dock-layout construction (headless, no GL context) |

Tests for the private OpenGL classes (`OpenGlShader`, `OpenGlMesh`,
`OpenGlTexture`, `OpenGlFramebuffer`) include their headers via the
`libskeleton/src` include directory, which the tests target adds privately.
`SKELETON_RES_DIR` (exported publicly by `libskeleton`) points tests at the
real shader files under `libskeleton/res/shaders`.

`skeledit_tests` compiles `skeledit/src/imguieditor.cc` and
`skeledit/src/editorlogsink.cc` directly and links `spdlog`, `imgui`, `glfw`,
and Google Test. It drives the dock builder API headlessly:
the test `SetUp` creates an ImGui context, enables
`ImGuiConfigFlags_DockingEnable`, sets a display size / delta time, and builds
the font atlas via `io.Fonts->Build()` so that `ImGui::NewFrame()` can run
without a renderer backend. Only the static, backend-free parts of the class
(`DockspaceId()`, `CreateDockLayout()`) are exercised, since the constructor
and frame/render methods need a real GL context. The `EditorLogSink` tests log
through a standalone `spdlog::logger` (not the default logger) so they run
without touching global spdlog state. They assert the entries match the shared
`kLogPattern` shape (gtest's `MatchesRegex` is limited, so timestamps are
matched with `\d` escapes and no `{n}` repetition).

## Running tests

Build and run through CTest:

```sh
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

or run the test executable directly:

```sh
build\tests\Release\libskeleton_tests.exe
```

## Policy

**All production code must be accompanied by unit tests.** New classes and
functions should add coverage in `tests/` before the work is considered done.
