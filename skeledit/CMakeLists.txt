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

add_executable(skeledit
    src/main.cc
    src/imguieditor.cc
    src/editorlogsink.cc
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES
    src/main.cc
    src/imguieditor.cc
    src/editorlogsink.cc
)

target_compile_features(skeledit PRIVATE cxx_std_20)

# The editor keeps its logging enabled in every configuration, so spdlog macros
# are never stripped out of release builds of skeledit.
target_compile_definitions(skeledit PRIVATE
    SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)

target_include_directories(skeledit
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(skeledit PRIVATE libskeleton imgui spdlog)

cpplint_add_target(skeledit ROOT skeledit/include)
