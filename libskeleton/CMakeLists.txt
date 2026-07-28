add_library(libskeleton STATIC
    src/skeleton.cpp
)

target_compile_features(libskeleton PUBLIC cxx_std_20)

target_include_directories(libskeleton
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
