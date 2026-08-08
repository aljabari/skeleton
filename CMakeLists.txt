cmake_minimum_required(VERSION 3.16)
project(skeleton VERSION 1.0.0 LANGUAGES CXX)

option(SKELETON_BUILD_TESTS "Build unit tests" ON)
option(SKELETON_ENABLE_LOGGING_IN_RELEASE
    "Compile spdlog logging into non-Debug configurations" OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
if (WIN32)
    include(platform/windows)
elseif (EMSCRIPTEN)
    # Included by absolute path: the Emscripten toolchain puts its own
    # Modules/Platform/Emscripten.cmake on the CMAKE_MODULE_PATH, which would
    # otherwise shadow this file on case-insensitive file systems.
    include(${CMAKE_CURRENT_LIST_DIR}/cmake/platform/emscripten.cmake)
endif()
include(CPPLint)
include(CTest)
include(dependencies)
include(version)

add_subdirectory(libskeleton)
add_subdirectory(skeleton)
add_subdirectory(skeledit)

if (SKELETON_BUILD_TESTS)
    add_subdirectory(tests)
endif()

if (WIN32)
    set_directory_properties(PROPERTIES VS_STARTUP_PROJECT skeleton)
endif()
