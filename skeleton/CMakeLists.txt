add_executable(skeleton
    src/main.cpp
    include/skeleton/main.h
)

target_compile_features(skeleton PRIVATE cxx_std_20)

target_include_directories(skeleton
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(skeleton PRIVATE libskeleton)

cpplint_add_target(skeleton ROOT skeleton/include)
