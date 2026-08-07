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
|                     | `tests/src/scene_test.cc`                     | `Scene` wrapping `entt::registry` (`Registry` accessor, entity/`MeshComponent`/`NameComponent` storage, empty default state) |
|                     | `tests/src/renderer/opengl/openglframebuffer_test.cc` | `OpenGlFramebuffer` allocation, binding, resizing, and cleanup |
|                     | `tests/src/renderer/opengl/openglrenderer_test.cc` | `OpenGlRenderer::CreateContext` window/context creation (3.3 context, current context, glad), rendering the scene's triangle mesh, render-target resizing, and viewport configuration |
|                     | `tests/src/renderer/opengl/openglshader_test.cc` | `OpenGlShader` loading and cross-compiling the shared SPIR-V modules (SPIRV-Cross produces `#version 330` desktop GLSL with the y-flip and clip-space fixup), compilation/linking, and program binding |
|                     | `tests/src/renderer/opengl/openglmesh_test.cc` | `OpenGlMesh` VAO binding and error-free drawing |
|                     | `tests/src/renderer/opengl/opengltexture_test.cc` | `OpenGlTexture` allocation, binding, and cleanup |
|                     | `tests/src/renderer/rendererfactory_test.cc` | `RendererBackend` enum, platform priority order, and fallback creation (`CreateRenderer` with a preferred backend) with injectable fake creators, including creators that return `nullptr` or throw `RendererCreationException`, and renderers whose `CreateContext` throws |
|                     | `tests/src/renderer/vulkan/vulkaninstance_test.cc` | `VulkanInstance` instance creation and physical-device enumeration (skips when Vulkan is unavailable); in Debug builds, debug-messenger creation for validation layers |
|                     | `tests/src/renderer/vulkan/vulkancommandbuffer_test.cc` | `VulkanCommandBuffer` command-pool/buffer creation from the graphics queue family (skips when Vulkan is unavailable) |
|                     | `tests/src/renderer/vulkan/vulkandevice_test.cc` | `VulkanDevice` logical-device/queue creation from a `VulkanInstance` and window surface, including the present queue (skips when Vulkan is unavailable) |
|                     | `tests/src/renderer/vulkan/vulkanfence_test.cc` | `VulkanFence` fence creation and initial-signaled state so the first `Render` wait does not block (skips when Vulkan is unavailable) |
|                     | `tests/src/renderer/vulkan/vulkanframebuffer_test.cc` | `VulkanFramebuffer` creation from a swapchain image view, render pass, and extent (skips when Vulkan is unavailable) |
|                     | `tests/src/renderer/vulkan/vulkangraphicspipeline_test.cc` | `VulkanGraphicsPipeline` pipeline/layout creation from the build-time compiled `triangle.vert.spv`/`triangle.frag.spv` shader modules against a hardcoded-format render pass, without a swapchain (skips when Vulkan is unavailable) |
|                     | `tests/src/renderer/vulkan/vulkaninstance_test.cc` | `VulkanInstance` instance creation and physical-device enumeration (skips when Vulkan is unavailable); in Debug builds, debug-messenger creation for validation layers |
|                     | `tests/src/renderer/vulkan/vulkanmesh_test.cc` | `VulkanMesh` vertex-buffer/memory creation from the same hardcoded triangle mesh the OpenGL renderer tests use (skips when Vulkan is unavailable) |
|                     | `tests/src/renderer/vulkan/vulkanrenderer_test.cc` | `VulkanRenderer::CreateContext` instance/surface/window/physical-device/logical-device/swapchain/render-pass/pipeline/mesh/framebuffer/command-buffer/synchronisation creation, `Render` drawing the scene's triangle mesh without error, rendering to a texture target (with the render-target image view/extent), `ResizeRenderTarget` recreating the image, and window targets having no render-target image (skips when Vulkan is unavailable) |
|                     | `tests/src/renderer/vulkan/vulkanrenderpass_test.cc` | `VulkanRenderPass` render-pass creation with a colour attachment and the given final image layout (skips when Vulkan is unavailable) |
|                     | `tests/src/renderer/vulkan/vulkanrendertarget_test.cc` | `VulkanRenderTarget` off-screen colour image/image-view creation with colour-attachment and sampled usage, at the requested size (skips when Vulkan is unavailable) |
|                     | `tests/src/renderer/vulkan/vulkansemaphore_test.cc` | `VulkanSemaphore` semaphore creation (skips when Vulkan is unavailable) |
|                     | `tests/src/renderer/vulkan/vulkanswapchain_test.cc` | `VulkanSwapchain` swapchain and image-view creation, including swapchain extent and format selection (skips when Vulkan is unavailable) |
| `skeledit_tests`    | `tests/src/skeledit/editorlogsink_test.cc` | `EditorLogSink` buffering, canonical timestamped format (`[YYYY-MM-DD HH:MM:SS.mmm] [level] message`), `Clear`, and `kMaxEntries` bound |
|                     | `tests/src/skeledit/imguieditor_test.cc`       | `ImGuiEditor` dock-layout construction (viewport, right-docked scene graph spanning full height with the entity panel docked below it, bottom logs; headless, no GL context) |

Tests for the private OpenGL classes (`OpenGlShader`, `OpenGlMesh`,
`OpenGlTexture`, `OpenGlFramebuffer`) include their headers via the
`libskeleton/src` include directory, which the tests target adds privately.
`SKELETON_SHADER_DIR` (exported publicly by `libskeleton`) points tests at the
build-time compiled SPIR-V shaders under
`${CMAKE_CURRENT_BINARY_DIR}/shaders`.

`skeledit_tests` compiles `skeledit/src/imguieditor.cc` and
`skeledit/src/editorlogsink.cc` directly and links `spdlog`, `imgui`, `glfw`,
and Google Test. It drives the dock builder API headlessly:
the test `SetUp` creates an ImGui context, enables
`ImGuiConfigFlags_DockingEnable`, sets a display size / delta time, and builds
the font atlas via `io.Fonts->Build()` so that `ImGui::NewFrame()` can run
without a renderer backend. Only the static, backend-free parts of the class
(`DockspaceId()`, `CreateDockLayout()`) are exercised, since the constructor
now takes a `std::unique_ptr<ImGuiBackend>` and the frame/render methods need a
real GL/Vulkan context. The `EditorLogSink` tests log
through a standalone `spdlog::logger` (not the default logger) so they run
without touching global spdlog state. They assert the entries match the shared
`kLogPattern` shape (gtest's `MatchesRegex` is limited, so timestamps are
matched with `\d` escapes and no `{n}` repetition).

The ImGui-backend refactor is verified at run time rather than by unit tests:
running `skeledit` picks the Vulkan renderer first, and the Vulkan backend
(`ImGui_ImplVulkan_Init` against its own render pass and descriptor pool, plus
the `FrameSubmitCallback` hook firing inside `VulkanRenderer::Render` to submit
the backend's command buffer) initialises and draws without validation-layer
errors in Debug builds. The render target is registered with ImGui through
`ImGui_ImplVulkan_AddTexture` (the image view ends the scene pass in
`VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL`) and displayed in the viewport via
`ImGui::Image`, so the off-screen scene previews in the editor. The backend's
`GetViewportTextureId` re-registers the descriptor set when the renderer
recreates the render target (for example on resize).
`VulkanRendererTest.FrameSubmitCallbackCanAddCommandBuffer`
unit-tests the hook itself: it registers a callback that returns a
begin/end-recorded secondary command buffer and asserts `Render()` submits it on
every recorded frame.

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
