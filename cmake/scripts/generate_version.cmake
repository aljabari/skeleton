# Copyright 2026 aljabari

# Build-time version header generator.
#
# Invoked with `cmake -P` by the libskeleton_version custom target on every
# build (see cmake/version.cmake). It derives the skeleton version from the git
# history of SKELETON_SOURCE_DIR and expands SKELETON_VERSION_TEMPLATE into
# SKELETON_VERSION_OUTPUT:
#
#   * SKELETON_VERSION_MAJOR / SKELETON_VERSION_MINOR come from the most recent
#     tag reachable from HEAD matching "v<major>.<minor>" (`git describe`).
#   * SKELETON_VERSION_PATCH is the number of commits since that tag
#     (`git rev-list --count <tag>..HEAD`).
#   * When no matching tag exists yet, the version is 0.<commit count>.
#   * When git is unavailable (e.g. a source archive without .git), the root
#     project version (SKELETON_VERSION_FALLBACK_*) is used.
#
# The header is only rewritten when its content actually changed, so an
# unchanged version does not trigger recompilation of files that include it.

set(_git_executable "${GIT_EXECUTABLE}")
if (NOT _git_executable)
    find_program(_git_executable NAMES git)
endif()

set(_major "${SKELETON_VERSION_FALLBACK_MAJOR}")
set(_minor "${SKELETON_VERSION_FALLBACK_MINOR}")
set(_patch "${SKELETON_VERSION_FALLBACK_PATCH}")

if (_git_executable AND EXISTS "${SKELETON_SOURCE_DIR}/.git")
    execute_process(
        COMMAND "${_git_executable}" describe --tags --abbrev=0
            --match v[0-9]*.[0-9]*
        WORKING_DIRECTORY "${SKELETON_SOURCE_DIR}"
        RESULT_VARIABLE _describe_result
        OUTPUT_VARIABLE _tag
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET)

    if (_describe_result EQUAL 0 AND _tag MATCHES "^v([0-9]+)\\.([0-9]+)$")
        set(_major "${CMAKE_MATCH_1}")
        set(_minor "${CMAKE_MATCH_2}")

        # Patch = number of commits made since the tagged commit.
        execute_process(
            COMMAND "${_git_executable}" rev-list --count "${_tag}..HEAD"
            WORKING_DIRECTORY "${SKELETON_SOURCE_DIR}"
            RESULT_VARIABLE _count_result
            OUTPUT_VARIABLE _patch
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET)
        if (NOT _count_result EQUAL 0)
            set(_patch "${SKELETON_VERSION_FALLBACK_PATCH}")
        endif()
    else()
        # No tag matching v<major>.<minor> yet: version is 0.0.<commit count>.
        set(_major 0)
        set(_minor 0)
        execute_process(
            COMMAND "${_git_executable}" rev-list --count HEAD
            WORKING_DIRECTORY "${SKELETON_SOURCE_DIR}"
            RESULT_VARIABLE _count_result
            OUTPUT_VARIABLE _patch
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET)
        if (NOT _count_result EQUAL 0)
            set(_patch "${SKELETON_VERSION_FALLBACK_PATCH}")
        endif()
    endif()
endif()

set(SKELETON_VERSION_MAJOR "${_major}")
set(SKELETON_VERSION_MINOR "${_minor}")
set(SKELETON_VERSION_PATCH "${_patch}")
set(SKELETON_VERSION_STRING "v${_major}.${_minor}.${_patch}")

file(READ "${SKELETON_VERSION_TEMPLATE}" _template)
string(CONFIGURE "${_template}" _generated @ONLY)

set(_rewrite TRUE)
if (EXISTS "${SKELETON_VERSION_OUTPUT}")
    file(READ "${SKELETON_VERSION_OUTPUT}" _current)
    if (_current STREQUAL _generated)
        set(_rewrite FALSE)
    endif()
endif()

if (_rewrite)
    file(WRITE "${SKELETON_VERSION_OUTPUT}" "${_generated}")
    message(STATUS "Generated ${SKELETON_VERSION_OUTPUT} "
        "(${SKELETON_VERSION_STRING})")
endif()
