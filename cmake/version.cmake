# Copyright 2026 aljabari

find_package(Git QUIET)

# skeleton_generate_version()
#
# Wires up build-time generation of the libskeleton version header. On every
# build the libskeleton_version custom target runs cmake/scripts/
# generate_version.cmake, which derives the version from the git history of the
# source directory and rewrites
# ${CMAKE_CURRENT_BINARY_DIR}/include/libskeleton/version.h from the template at
# include/libskeleton/version.h.in (only when the content changed). The
# generated include directory is exported PUBLIC so every consumer of
# libskeleton sees version.h.
#
# Version derivation (see cmake/scripts/generate_version.cmake for details):
#   * SKELETON_VERSION_MAJOR / SKELETON_VERSION_MINOR come from the most recent
#     tag reachable from HEAD matching "v<major>.<minor>".
#   * SKELETON_VERSION_PATCH is the number of commits since that tag.
#   * When no matching tag exists yet, the version is 0.<commit count>.
#   * When git is unavailable, the root project version from
#     project(skeleton VERSION ...) is used.
#
# Must be called from libskeleton/CMakeLists.txt after add_library(libskeleton).

function(skeleton_generate_version)
    if (NOT DEFINED PROJECT_VERSION_MAJOR)
        set(PROJECT_VERSION_MAJOR 0)
        set(PROJECT_VERSION_MINOR 0)
        set(PROJECT_VERSION_PATCH 0)
    endif()

    set(_version_template
        "${CMAKE_CURRENT_SOURCE_DIR}/include/libskeleton/version.h.in")
    set(_version_output
        "${CMAKE_CURRENT_BINARY_DIR}/include/libskeleton/version.h")

    add_custom_target(libskeleton_version ALL
        COMMAND ${CMAKE_COMMAND}
            -DGIT_EXECUTABLE="${GIT_EXECUTABLE}"
            -DSKELETON_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
            -DSKELETON_VERSION_TEMPLATE="${_version_template}"
            -DSKELETON_VERSION_OUTPUT="${_version_output}"
            -DSKELETON_VERSION_FALLBACK_MAJOR="${PROJECT_VERSION_MAJOR}"
            -DSKELETON_VERSION_FALLBACK_MINOR="${PROJECT_VERSION_MINOR}"
            -DSKELETON_VERSION_FALLBACK_PATCH="${PROJECT_VERSION_PATCH}"
            -P "${CMAKE_SOURCE_DIR}/cmake/scripts/generate_version.cmake"
        BYPRODUCTS "${_version_output}"
        COMMENT "Generating version header from git history")

    add_dependencies(libskeleton libskeleton_version)

    target_include_directories(libskeleton PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>)
endfunction()
