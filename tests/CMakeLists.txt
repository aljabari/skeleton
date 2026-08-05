add_executable(libskeleton_tests
    src/window_test.cc
    src/renderer/opengl/openglframebuffer_test.cc
    src/renderer/opengl/openglmesh_test.cc
    src/renderer/opengl/openglrenderer_test.cc
    src/renderer/opengl/openglshader_test.cc
    src/renderer/opengl/opengltexture_test.cc
    src/renderer/rendererfactory_test.cc
    src/renderer/vulkan/vulkancommandbuffer_test.cc
    src/renderer/vulkan/vulkandevice_test.cc
    src/renderer/vulkan/vulkanfence_test.cc
    src/renderer/vulkan/vulkanframebuffer_test.cc
    src/renderer/vulkan/vulkangraphicspipeline_test.cc
    src/renderer/vulkan/vulkaninstance_test.cc
    src/renderer/vulkan/vulkanmesh_test.cc
    src/renderer/vulkan/vulkanrenderer_test.cc
    src/renderer/vulkan/vulkanrenderpass_test.cc
    src/renderer/vulkan/vulkanrendertarget_test.cc
    src/renderer/vulkan/vulkansemaphore_test.cc
    src/renderer/vulkan/vulkanswapchain_test.cc
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES
    src/window_test.cc
    src/renderer/opengl/openglframebuffer_test.cc
    src/renderer/opengl/openglmesh_test.cc
    src/renderer/opengl/openglrenderer_test.cc
    src/renderer/opengl/openglshader_test.cc
    src/renderer/opengl/opengltexture_test.cc
    src/renderer/rendererfactory_test.cc
    src/renderer/vulkan/vulkancommandbuffer_test.cc
    src/renderer/vulkan/vulkandevice_test.cc
    src/renderer/vulkan/vulkanfence_test.cc
    src/renderer/vulkan/vulkanframebuffer_test.cc
    src/renderer/vulkan/vulkangraphicspipeline_test.cc
    src/renderer/vulkan/vulkaninstance_test.cc
    src/renderer/vulkan/vulkanmesh_test.cc
    src/renderer/vulkan/vulkanrenderer_test.cc
    src/renderer/vulkan/vulkanrenderpass_test.cc
    src/renderer/vulkan/vulkanrendertarget_test.cc
    src/renderer/vulkan/vulkansemaphore_test.cc
    src/renderer/vulkan/vulkanswapchain_test.cc
)

target_compile_features(libskeleton_tests PRIVATE cxx_std_20)

# Match libskeleton: strip spdlog logging in non-Debug builds unless
# SKELETON_ENABLE_LOGGING_IN_RELEASE is ON.
target_compile_definitions(libskeleton_tests PRIVATE
    SPDLOG_ACTIVE_LEVEL=$<IF:$<OR:$<CONFIG:Debug>,$<BOOL:${SKELETON_ENABLE_LOGGING_IN_RELEASE}>>,SPDLOG_LEVEL_TRACE,SPDLOG_LEVEL_OFF>)

target_include_directories(libskeleton_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../libskeleton/src
)

target_link_libraries(libskeleton_tests
    PRIVATE
        libskeleton
        GTest::gtest_main
        GTest::gmock
)

include(GoogleTest)
gtest_discover_tests(libskeleton_tests)

cpplint_add_target(libskeleton_tests ROOT ${CMAKE_SOURCE_DIR})

add_executable(skeledit_tests
    src/skeledit/editorlogsink_test.cc
    src/skeledit/imguieditor_test.cc
    ../skeledit/src/editorlogsink.cc
    ../skeledit/src/imguieditor.cc
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES
    src/skeledit/editorlogsink_test.cc
    src/skeledit/imguieditor_test.cc
)
source_group("Source Files\\skeledit" FILES
    ../skeledit/src/editorlogsink.cc
    ../skeledit/src/imguieditor.cc
)

target_compile_features(skeledit_tests PRIVATE cxx_std_20)

# Match skeledit: keep spdlog logging enabled in every configuration.
target_compile_definitions(skeledit_tests PRIVATE
    SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)

target_include_directories(skeledit_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../skeledit/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../libskeleton/include
)

target_link_libraries(skeledit_tests
    PRIVATE
        spdlog
        imgui
        glfw
        GTest::gtest_main
        GTest::gmock
)

include(GoogleTest)
gtest_discover_tests(skeledit_tests)

cpplint_add_target(skeledit_tests ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../skeledit/include)
