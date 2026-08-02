add_executable(skeledit
    src/main.cc
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES src/main.cc)

target_compile_features(skeledit PRIVATE cxx_std_20)

target_include_directories(skeledit
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(skeledit PRIVATE libskeleton)

cpplint_add_target(skeledit ROOT skeledit/include)
