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

add_library(imgui STATIC ${_imgui_sources})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${_imgui_sources})

target_compile_features(imgui PUBLIC cxx_std_20)

target_include_directories(imgui
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
