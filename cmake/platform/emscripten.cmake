# Platform capability flags for the target operating system.
# Only included from the root CMakeLists.txt when the target is Emscripten.

# Emscripten disables C++ exception catching by default (it costs size and
# speed). The renderer factory catches RendererCreationException to fall back
# between backends, so keep exception handling enabled across all targets.
add_compile_options(-sDISABLE_EXCEPTION_CATCHING=0)

# WebGL 2 note: emscripten defaults MIN_WEBGL_VERSION/MAX_WEBGL_VERSION to 1,
# so the WebGL context would be created as WebGL 1 and reject the ES 3.0
# shaders ("unsupported shader version 300"). The flags are link-time settings,
# so they are pinned to 2 in the skeleton target's link options (see
# skeleton/CMakeLists.txt), not here, to avoid em++ warnings during compilation.

# OpenGL is supported through WebGL 2, which exposes OpenGL ES 3.0.
set(SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL TRUE)

# OpenGL is supported through WebGL 2, which exposes OpenGL ES 3.0.
set(SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL TRUE)

# Vulkan is not available in the browser.
set(SKELETON_TARGET_SUPPORTS_RENDERER_VULKAN FALSE)
