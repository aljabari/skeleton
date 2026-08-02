add_executable(libskeleton_tests
    src/window_test.cc
    src/renderer/opengl/openglrenderer_test.cc
    src/renderer/vulkan/vulkanrenderer_test.cc
)

target_compile_features(libskeleton_tests PRIVATE cxx_std_20)

target_link_libraries(libskeleton_tests
    PRIVATE
        libskeleton
        GTest::gtest_main
        GTest::gmock
)

include(GoogleTest)
gtest_discover_tests(libskeleton_tests)

cpplint_add_target(libskeleton_tests ROOT ${CMAKE_SOURCE_DIR})
