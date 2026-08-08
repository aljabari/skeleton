include(FetchContent)

if (EMSCRIPTEN)
    # Dependencies such as spdlog call find_package(Threads REQUIRED), but the
    # pthread probes never pass under Emscripten (there is no pthread library
    # and -pthread is not accepted by emcc by default). FindThreads resets
    # Threads_FOUND before probing, so pre-seed the libc probe result instead:
    # its cached value is returned without re-running the probe, which marks
    # Threads found with an empty CMAKE_THREAD_LIBS_INIT (a no-op
    # Threads::Threads, so the web build does not require SharedArrayBuffer /
    # cross-origin isolation).
    set(CMAKE_HAVE_LIBC_PTHREAD TRUE CACHE INTERNAL "" FORCE)
endif()

if (EMSCRIPTEN)
    # GLFW cannot be built from source for Emscripten (no supported platform
    # backend), so expose the GLFW port shipped with the Emscripten SDK
    # (-sUSE_GLFW=3, a JavaScript implementation of the GLFW 3 API) as an
    # INTERFACE target with the same name that the rest of the build links.
    add_library(glfw INTERFACE)
    target_include_directories(glfw INTERFACE "${EMSCRIPTEN_SYSROOT}/include")
    target_link_options(glfw INTERFACE "-sUSE_GLFW=3")
else()
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
    )
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(glfw)
endif()

if (SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL AND NOT EMSCRIPTEN)
    # glad loads desktop GL function pointers on native platforms. Emscripten
    # links the WebGL 2 / OpenGL ES 3.0 entry points directly, so it is skipped
    # there.
    FetchContent_Declare(
        glad
        GIT_REPOSITORY https://github.com/Dav1dde/glad.git
        GIT_TAG v2.0.6
        SOURCE_SUBDIR cmake
    )
    FetchContent_MakeAvailable(glad)
endif()

if (SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL)
    # The OpenGL renderer loads the SPIR-V shaders at run time and
    # cross-compiles them to desktop GLSL with spirv-cross. Only the GLSL
    # backend is needed, so the other backends, the CLI, and the test suite
    # are disabled to keep the build small.
    set(SPIRV_CROSS_CLI OFF CACHE BOOL "" FORCE)
    set(SPIRV_CROSS_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
    set(SPIRV_CROSS_ENABLE_MSL OFF CACHE BOOL "" FORCE)
    set(SPIRV_CROSS_ENABLE_HLSL OFF CACHE BOOL "" FORCE)
    set(SPIRV_CROSS_ENABLE_CPP OFF CACHE BOOL "" FORCE)
    set(SPIRV_CROSS_ENABLE_REFLECT OFF CACHE BOOL "" FORCE)
    set(SPIRV_CROSS_ENABLE_C_API OFF CACHE BOOL "" FORCE)
    set(SPIRV_CROSS_ENABLE_UTIL OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        spirv-cross
        GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Cross.git
        GIT_TAG vulkan-sdk-1.4.350.1
    )
    FetchContent_MakeAvailable(spirv-cross)
endif()

if (SKELETON_TARGET_SUPPORTS_RENDERER_VULKAN)
    # Vulkan headers are needed to compile volk; volk also pulls them into its
    # own PUBLIC include directories (VOLK_PULL_IN_VULKAN is ON by default).
    find_package(Vulkan)
    FetchContent_Declare(
        volk
        GIT_REPOSITORY https://github.com/zeux/volk.git
        GIT_TAG 1.4.350
    )
    FetchContent_MakeAvailable(volk)
endif()

FetchContent_Declare(
    entt
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG v3.16.0
)
FetchContent_MakeAvailable(entt)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.17.0
)
set(SPDLOG_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(spdlog)

if (SKELETON_BUILD_TESTS)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.17.0
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()
