cmake_minimum_required(VERSION 3.16)
project(skeleton VERSION 1.0.0 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(CPPLint)
include(Dependencies)

add_subdirectory(libskeleton)
add_subdirectory(skeleton)

if (WIN32)
    set_directory_properties(PROPERTIES VS_STARTUP_PROJECT skeleton)
endif()
