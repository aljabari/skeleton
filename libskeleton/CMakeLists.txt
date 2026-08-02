if (SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL)
    set(_renderer_opengl_sources
        src/renderer/opengl/openglrenderer.cc
        include/libskeleton/opengl/openglrenderer.h
    )
endif()

add_library(libskeleton STATIC
    src/window.cc
    src/renderer/renderer.cc
    include/libskeleton/window.h
    include/libskeleton/renderer.h
    ${_renderer_opengl_sources}
)

target_compile_features(libskeleton PUBLIC cxx_std_20)

if (SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL)
    target_compile_definitions(libskeleton
        PUBLIC SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL)
endif()

target_include_directories(libskeleton
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(libskeleton PUBLIC glfw)

cpplint_add_target(libskeleton ROOT libskeleton/include)
