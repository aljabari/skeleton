add_library(libskeleton STATIC
    src/window.cpp
    include/libskeleton/window.h
)

target_compile_features(libskeleton PUBLIC cxx_std_20)

target_include_directories(libskeleton
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(libskeleton PUBLIC glfw)

cpplint_add_target(libskeleton ROOT libskeleton/include)
