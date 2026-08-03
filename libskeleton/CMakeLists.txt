if (SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL)
    set(_renderer_opengl_sources
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
        src/renderer/vulkan/vulkanrenderer.cc
        include/libskeleton/vulkan/vulkanrenderer.h
    )
endif()

set(_libskeleton_sources
    src/window.cc
    src/renderer/renderer.cc
    include/libskeleton/window.h
    include/libskeleton/renderer.h
    ${_renderer_opengl_sources}
    ${_renderer_vulkan_sources}
)

add_library(libskeleton STATIC ${_libskeleton_sources})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${_libskeleton_sources})

target_compile_features(libskeleton PUBLIC cxx_std_20)

if (SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL)
    target_compile_definitions(libskeleton
        PUBLIC
            SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL
            SKELETON_RES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/res")
endif()

if (SKELETON_TARGET_SUPPORTS_RENDERER_VULKAN)
    target_compile_definitions(libskeleton
        PUBLIC SKELETON_TARGET_SUPPORTS_RENDERER_VULKAN)
endif()

target_include_directories(libskeleton
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(libskeleton PUBLIC glfw)

if (SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL)
    glad_add_library(glad STATIC API gl:core=3.3 LOCATION ${CMAKE_CURRENT_BINARY_DIR}/glad)
    if (MSVC)
        target_compile_options(glad PRIVATE $<$<CONFIG:Debug>:/MDd>$<$<NOT:$<CONFIG:Debug>>:/MD>)
    endif()
    target_link_libraries(libskeleton PUBLIC glad)
endif()

cpplint_add_target(libskeleton ROOT libskeleton/include)
