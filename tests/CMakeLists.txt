add_executable(libskeleton_tests
    src/window_test.cc
    src/renderer/opengl/openglframebuffer_test.cc
    src/renderer/opengl/openglmesh_test.cc
    src/renderer/opengl/openglrenderer_test.cc
    src/renderer/opengl/openglshader_test.cc
    src/renderer/opengl/opengltexture_test.cc
    src/renderer/vulkan/vulkanrenderer_test.cc
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES
    src/window_test.cc
    src/renderer/opengl/openglframebuffer_test.cc
    src/renderer/opengl/openglmesh_test.cc
    src/renderer/opengl/openglrenderer_test.cc
    src/renderer/opengl/openglshader_test.cc
    src/renderer/opengl/opengltexture_test.cc
    src/renderer/vulkan/vulkanrenderer_test.cc
)

target_compile_features(libskeleton_tests PRIVATE cxx_std_20)

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
    src/skeledit/imguieditor_test.cc
    ../skeledit/src/imguieditor.cc
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES
    src/skeledit/imguieditor_test.cc
)
source_group("Source Files\\skeledit" FILES ../skeledit/src/imguieditor.cc)

target_compile_features(skeledit_tests PRIVATE cxx_std_20)

target_include_directories(skeledit_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../skeledit/include
)

target_link_libraries(skeledit_tests
    PRIVATE
        imgui
        glfw
        GTest::gtest_main
        GTest::gmock
)

include(GoogleTest)
gtest_discover_tests(skeledit_tests)

cpplint_add_target(skeledit_tests ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../skeledit/include)
