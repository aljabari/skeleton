if (SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL)
    set(_renderer_opengl_sources
        src/renderer/opengl/openglframebuffer.cc
        src/renderer/opengl/openglframebuffer.h
        src/renderer/opengl/openglmesh.cc
        src/renderer/opengl/openglmesh.h
        src/renderer/opengl/openglrenderer.cc
        include/libskeleton/opengl/openglrenderer.h
        src/renderer/opengl/openglshader.cc
        src/renderer/opengl/openglshader.h
        src/renderer/opengl/opengltexture.cc
        src/renderer/opengl/opengltexture.h
    )
endif()

if (SKELETON_TARGET_SUPPORTS_RENDERER_VULKAN)
    set(_renderer_vulkan_sources
        src/renderer/vulkan/vulkandevice.cc
        src/renderer/vulkan/vulkandevice.h
        src/renderer/vulkan/vulkangraphicspipeline.cc
        src/renderer/vulkan/vulkangraphicspipeline.h
        src/renderer/vulkan/vulkaninstance.cc
        src/renderer/vulkan/vulkaninstance.h
        src/renderer/vulkan/vulkanmesh.cc
        src/renderer/vulkan/vulkanmesh.h
        src/renderer/vulkan/vulkanswapchain.cc
        src/renderer/vulkan/vulkanswapchain.h
        src/renderer/vulkan/vulkanvalidation.cc
        src/renderer/vulkan/vulkanvalidation.h
        src/renderer/vulkan/vulkanrenderer.cc
        include/libskeleton/vulkan/vulkanrenderer.h
    )

    # Compile the GLSL shaders to SPIR-V at build time with glslc. The .spv
    # files are written into the build tree and loaded at run time by
    # VulkanGraphicsPipeline; SKELETON_VULKAN_SHADER_DIR points libskeleton (and
    # its consumers, including the tests) at that directory.
    find_program(SKELETON_GLSLC_EXECUTABLE NAMES glslc)
    if (NOT SKELETON_GLSLC_EXECUTABLE)
        message(FATAL_ERROR
            "glslc (the Vulkan SPIR-V compiler) was not found; install the "
            "Vulkan SDK or add glslc to PATH.")
    endif()
    set(_vulkan_shader_dir "${CMAKE_CURRENT_BINARY_DIR}/shaders")
    set(_vulkan_shader_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/res/shaders/triangle.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/res/shaders/triangle.frag)
    set(_vulkan_spv_outputs)
    foreach(_vulkan_shader IN LISTS _vulkan_shader_sources)
        get_filename_component(_vulkan_shader_name "${_vulkan_shader}" NAME)
        list(APPEND _vulkan_spv_outputs
            "${_vulkan_shader_dir}/${_vulkan_shader_name}.spv")
        add_custom_command(
            OUTPUT "${_vulkan_shader_dir}/${_vulkan_shader_name}.spv"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${_vulkan_shader_dir}"
            COMMAND "${SKELETON_GLSLC_EXECUTABLE}" -fauto-map-locations
                "${_vulkan_shader}"
                -o "${_vulkan_shader_dir}/${_vulkan_shader_name}.spv"
            DEPENDS "${_vulkan_shader}"
            VERBATIM)
    endforeach()
    add_custom_target(libskeleton_shaders DEPENDS ${_vulkan_spv_outputs})
endif()

set(_libskeleton_sources
    src/window.cc
    src/renderer/renderer.cc
    src/renderer/rendererfactory.cc
    include/libskeleton/logging.h
    include/libskeleton/rendererfactory.h
    include/libskeleton/window.h
    include/libskeleton/renderer.h
    ${_renderer_opengl_sources}
    ${_renderer_vulkan_sources}
)

add_library(libskeleton STATIC ${_libskeleton_sources})

if (SKELETON_TARGET_SUPPORTS_RENDERER_VULKAN)
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
            SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL
            SKELETON_RES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/res")
endif()

if (SKELETON_TARGET_SUPPORTS_RENDERER_VULKAN)
    target_compile_definitions(libskeleton
        PUBLIC
            SKELETON_TARGET_SUPPORTS_RENDERER_VULKAN
            SKELETON_VULKAN_ENABLE_VALIDATION=$<IF:$<CONFIG:Debug>,1,0>
            SKELETON_VULKAN_SHADER_DIR="${_vulkan_shader_dir}")
endif()

target_include_directories(libskeleton
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(libskeleton PUBLIC glfw spdlog)

if (SKELETON_TARGET_SUPPORTS_RENDERER_VULKAN)
    target_link_libraries(libskeleton PUBLIC volk)
endif()

if (SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL)
    glad_add_library(glad STATIC API gl:core=3.3 LOCATION ${CMAKE_CURRENT_BINARY_DIR}/glad)
    if (MSVC)
        target_compile_options(glad PRIVATE $<$<CONFIG:Debug>:/MDd>$<$<NOT:$<CONFIG:Debug>>:/MD>)
    endif()
    target_link_libraries(libskeleton PUBLIC glad)
endif()

cpplint_add_target(libskeleton ROOT libskeleton/include)
