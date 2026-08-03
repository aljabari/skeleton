# CMake build script for Dear ImGui (https://github.com/ocornut/imgui).
# The upstream repository intentionally ships no CMakeLists.txt, so this file
# is provided by this project. skeledit/CMakeLists.txt copies it into the
# fetched imgui source tree before adding it as a subdirectory.

set(_imgui_sources
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
)

set(_imgui_backend_sources
    backends/imgui_impl_glfw.cpp
    backends/imgui_impl_glfw.h
)

if (SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL)
    list(APPEND _imgui_backend_sources
        backends/imgui_impl_opengl3.cpp
        backends/imgui_impl_opengl3.h
    )
endif()

if (SKELETON_TARGET_SUPPORTS_RENDERER_VULKAN)
    list(APPEND _imgui_backend_sources
        backends/imgui_impl_vulkan.cpp
        backends/imgui_impl_vulkan.h
    )
endif()

set(_imgui_all_sources ${_imgui_sources} ${_imgui_backend_sources})

add_library(imgui STATIC ${_imgui_all_sources})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${_imgui_all_sources})

target_compile_features(imgui PUBLIC cxx_std_20)

target_include_directories(imgui
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/backends
)

target_link_libraries(imgui PRIVATE glfw)

if (SKELETON_TARGET_SUPPORTS_RENDERER_VULKAN)
    target_compile_definitions(imgui PRIVATE IMGUI_IMPL_VULKAN_USE_VOLK)
    target_link_libraries(imgui PRIVATE volk)
endif()
