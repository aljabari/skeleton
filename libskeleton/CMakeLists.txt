# Sources are collected with a recursive glob instead of an explicit list, so
# new files under src/ and include/ show up in the IDE (Visual Studio) and are
# compiled without editing this file. CONFIGURE_DEPENDS re-runs the glob and
# re-configures whenever the file set changes. Sources for a renderer backend
# the target platform does not support are filtered out below.
file(GLOB_RECURSE _libskeleton_sources CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
)

if (NOT SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL)
    list(FILTER _libskeleton_sources EXCLUDE REGEX "(^|/)opengl(_|/|$)")
endif()

if (NOT SKELETON_TARGET_SUPPORTS_RENDERER_VULKAN)
    list(FILTER _libskeleton_sources EXCLUDE REGEX "(^|/)vulkan(_|/|$)")
endif()

if (SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL OR
    SKELETON_TARGET_SUPPORTS_RENDERER_VULKAN)
    # Compile the Vulkan-style GLSL shaders to SPIR-V at build time with
    # glslc. The .spv files are written into the build tree and loaded at run
    # time by VulkanGraphicsPipeline directly and by OpenGlShader (which
    # cross-compiles them back to desktop GLSL with spirv-cross). Both the
    # input/output locations are explicit in the shaders, so no auto-mapping is
    # needed. SKELETON_SHADER_DIR points libskeleton (and its consumers,
    # including the tests) at the compiled shaders.
    find_program(SKELETON_GLSLC_EXECUTABLE NAMES glslc)
    if (NOT SKELETON_GLSLC_EXECUTABLE)
        message(FATAL_ERROR
            "glslc (the Vulkan SPIR-V compiler) was not found; install the "
            "Vulkan SDK or add glslc to PATH.")
    endif()
    set(_shader_dir "${CMAKE_CURRENT_BINARY_DIR}/shaders")
    set(_shader_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/res/shaders/triangle.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/res/shaders/triangle.frag)
    set(_spv_outputs)
    foreach(_shader IN LISTS _shader_sources)
        get_filename_component(_shader_name "${_shader}" NAME)
        list(APPEND _spv_outputs "${_shader_dir}/${_shader_name}.spv")
        add_custom_command(
            OUTPUT "${_shader_dir}/${_shader_name}.spv"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${_shader_dir}"
            COMMAND "${SKELETON_GLSLC_EXECUTABLE}" "${_shader}"
                -o "${_shader_dir}/${_shader_name}.spv"
            DEPENDS "${_shader}"
            VERBATIM)
    endforeach()
    add_custom_target(libskeleton_shaders DEPENDS ${_spv_outputs})
endif()

add_library(libskeleton STATIC ${_libskeleton_sources})

if (SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL OR
    SKELETON_TARGET_SUPPORTS_RENDERER_VULKAN)
    add_dependencies(libskeleton libskeleton_shaders)
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${_libskeleton_sources})

target_compile_features(libskeleton PUBLIC cxx_std_20)

# spdlog macros compile in only messages at or above SPDLOG_ACTIVE_LEVEL. Logging
# is fully compiled in for Debug builds and, unless
# SKELETON_ENABLE_LOGGING_IN_RELEASE is ON, stripped for all other
# configurations (Release etc.).
target_compile_definitions(libskeleton PRIVATE
    SPDLOG_ACTIVE_LEVEL=$<IF:$<OR:$<CONFIG:Debug>,$<BOOL:${SKELETON_ENABLE_LOGGING_IN_RELEASE}>>,SPDLOG_LEVEL_TRACE,SPDLOG_LEVEL_OFF>)

if (SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL)
    target_compile_definitions(libskeleton
        PUBLIC
            SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL)
endif()

if (SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL OR
    SKELETON_TARGET_SUPPORTS_RENDERER_VULKAN)
    if (EMSCRIPTEN)
        # There is no native filesystem in the browser. The skeleton target
        # embeds the compiled .spv files into the module at /shaders
        # (--embed-file), so SKELETON_SHADER_DIR points at that virtual
        # directory instead of the host build tree.
        set(_shader_dir "/shaders")
    endif()
    target_compile_definitions(libskeleton
        PUBLIC
            SKELETON_SHADER_DIR="${_shader_dir}")
endif()

if (SKELETON_TARGET_SUPPORTS_RENDERER_VULKAN)
    target_compile_definitions(libskeleton
        PUBLIC
            SKELETON_TARGET_SUPPORTS_RENDERER_VULKAN
            SKELETON_VULKAN_ENABLE_VALIDATION=$<IF:$<CONFIG:Debug>,1,0>)
endif()

target_include_directories(libskeleton
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(libskeleton PUBLIC glfw spdlog EnTT)

if (SKELETON_TARGET_SUPPORTS_RENDERER_VULKAN)
    target_link_libraries(libskeleton PUBLIC volk)
endif()

if (SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL)
    target_link_libraries(libskeleton PUBLIC spirv-cross-glsl)
    if (NOT EMSCRIPTEN)
        glad_add_library(glad STATIC API gl:core=3.3 LOCATION ${CMAKE_CURRENT_BINARY_DIR}/glad)
        if (MSVC)
            target_compile_options(glad PRIVATE $<$<CONFIG:Debug>:/MDd>$<$<NOT:$<CONFIG:Debug>>:/MD>)
        endif()
        target_link_libraries(libskeleton PUBLIC glad)
    endif()
endif()

cpplint_add_target(libskeleton ROOT libskeleton/include)

# Generates include/libskeleton/version.h in the build tree on every build,
# deriving the version from the git history (most recent v<major>.<minor> tag
# plus the number of commits since it). Must come after add_library.
skeleton_generate_version()
