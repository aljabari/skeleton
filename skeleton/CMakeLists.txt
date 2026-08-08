add_executable(skeleton
    src/main.cc
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES src/main.cc)

target_compile_features(skeleton PRIVATE cxx_std_20)

# Match libskeleton: strip spdlog logging in non-Debug builds unless
# SKELETON_ENABLE_LOGGING_IN_RELEASE is ON.
target_compile_definitions(skeleton PRIVATE
    SPDLOG_ACTIVE_LEVEL=$<IF:$<OR:$<CONFIG:Debug>,$<BOOL:${SKELETON_ENABLE_LOGGING_IN_RELEASE}>>,SPDLOG_LEVEL_TRACE,SPDLOG_LEVEL_OFF>)

if (WIN32)
    # Release builds run as a GUI application without a console window. Debug
    # keeps the console so log output is visible while developing.
    set_target_properties(skeleton PROPERTIES
        WIN32_EXECUTABLE "$<IF:$<CONFIG:Release>,ON,OFF>")
    if (MSVC)
        # The GUI subsystem makes MSVC expect WinMain; route it back to the
        # app's main() via the CRT startup entry point.
        target_link_options(skeleton PRIVATE
            "$<$<CONFIG:Release>:/ENTRY:mainCRTStartup>")
    endif()
endif()

target_include_directories(skeleton
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

if (EMSCRIPTEN)
    # Name the output with an .html extension so emcc also emits a self-
    # contained HTML shell (skeleton.html plus skeleton.js/skeleton.wasm) that
    # runs the app in a browser.
    set_target_properties(skeleton PROPERTIES SUFFIX ".html")
    # Enable emrun support so scripts\html5\run_emscripten.bat can serve the
    # page, forward browser stdout, and close the server when the app exits.
    # Also keep exception catching enabled (matching the global compile option
    # in cmake\platform\emscripten.cmake) so the renderer factory can fall back
    # between backends instead of aborting, and embed the SPIR-V shaders built
    # by libskeleton into the virtual filesystem at /shaders (where
    # SKELETON_SHADER_DIR points on this platform). MIN/MAX_WEBGL_VERSION pin
    # the WebGL context to version 2 so the GLFW Emscripten port (which ignores
    # the GLFW version hints) creates a WebGL 2 context for the ES 3.0 shaders.
    # These are link-time settings, so they live here rather than in
    # cmake\platform\emscripten.cmake (which would make em++ warn).
    target_link_options(skeleton PRIVATE
        --emrun
        -sDISABLE_EXCEPTION_CATCHING=0
        -sMIN_WEBGL_VERSION=2
        -sMAX_WEBGL_VERSION=2
        "--embed-file=${CMAKE_BINARY_DIR}/libskeleton/shaders@/shaders")
endif()

target_link_libraries(skeleton PRIVATE libskeleton)

cpplint_add_target(skeleton ROOT skeleton/include)
