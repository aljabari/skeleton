# Test sources are collected with a recursive glob instead of an explicit list,
# so new test files show up in the IDE (Visual Studio) and are compiled without
# editing this file. CONFIGURE_DEPENDS re-runs the glob and re-configures
# whenever the file set changes. The skeledit tests are excluded here because
# they build into the skeledit_tests target below.
file(GLOB_RECURSE _libskeleton_tests_sources CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*_test.cc
)

list(FILTER _libskeleton_tests_sources EXCLUDE REGEX "(^|/)skeledit(/|$)")

add_executable(libskeleton_tests ${_libskeleton_tests_sources})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES
    ${_libskeleton_tests_sources}
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

file(GLOB_RECURSE _skeledit_tests_sources CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/skeledit/*_test.cc
)

# skeledit_tests compiles the skeledit sources it exercises directly, so the
# headless-testable logic can be tested without a GL context (see
# knowledge/BUILD_SYSTEM.md). These stay explicit: globbing all of
# ../skeledit/src would also pick up main.cc and the renderer backend sources.
set(_skeledit_tests_skeledit_sources
    ../skeledit/src/editorlogsink.cc
    ../skeledit/src/imguieditor.cc
)

add_executable(skeledit_tests ${_skeledit_tests_sources} ${_skeledit_tests_skeledit_sources})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES
    ${_skeledit_tests_sources}
)
source_group("Source Files\\skeledit" FILES
    ${_skeledit_tests_skeledit_sources}
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
        libskeleton
        imgui
        GTest::gtest_main
        GTest::gmock
)

include(GoogleTest)
gtest_discover_tests(skeledit_tests)

cpplint_add_target(skeledit_tests ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../skeledit/include)
