include(FetchContent)

FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
    FetchContent_Populate(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG docking
        GIT_SHALLOW ON
    )
    file(COPY "${CMAKE_SOURCE_DIR}/cmake/third_party/imgui/CMakeLists.txt"
         DESTINATION "${imgui_SOURCE_DIR}")
    add_subdirectory(${imgui_SOURCE_DIR} ${imgui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

set(_skeledit_sources
    src/main.cc
    src/imguieditor.cc
    src/editorlogsink.cc
)

if (SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL)
    list(APPEND _skeledit_sources src/opengl_imguibackend.cc)
endif()

if (SKELETON_TARGET_SUPPORTS_RENDERER_VULKAN)
    list(APPEND _skeledit_sources src/vulkan_imguibackend.cc)
endif()

if (SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL OR
    SKELETON_TARGET_SUPPORTS_RENDERER_VULKAN)
    list(APPEND _skeledit_sources src/imguibackendfactory.cc)
endif()

add_executable(skeledit ${_skeledit_sources})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES
    ${_skeledit_sources}
)

target_compile_features(skeledit PRIVATE cxx_std_20)

# The editor keeps its logging enabled in every configuration, so spdlog macros
# are never stripped out of release builds of skeledit.
target_compile_definitions(skeledit PRIVATE
    SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)

if (SKELETON_TARGET_SUPPORTS_RENDERER_VULKAN)
    # skeledit includes imgui_impl_vulkan.h itself, which switches to volk
    # only when this define is set (the imgui library sets it PRIVATE).
    target_compile_definitions(skeledit PRIVATE
        IMGUI_IMPL_VULKAN_USE_VOLK)
endif()

if (WIN32)
    # Release builds run as a GUI application without a console window. Debug
    # keeps the console so log output is visible while developing.
    set_target_properties(skeledit PROPERTIES
        WIN32_EXECUTABLE "$<IF:$<CONFIG:Release>,ON,OFF>")
    if (MSVC)
        # The GUI subsystem makes MSVC expect WinMain; route it back to the
        # app's main() via the CRT startup entry point.
        target_link_options(skeledit PRIVATE
            "$<$<CONFIG:Release>:/ENTRY:mainCRTStartup>")
    endif()
endif()

target_include_directories(skeledit
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(skeledit PRIVATE libskeleton imgui spdlog)

cpplint_add_target(skeledit ROOT skeledit/include)
