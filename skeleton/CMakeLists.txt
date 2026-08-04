add_executable(skeleton
    src/main.cc
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES src/main.cc)

target_compile_features(skeleton PRIVATE cxx_std_20)

# Match libskeleton: strip spdlog logging in non-Debug builds.
target_compile_definitions(skeleton PRIVATE
    SPDLOG_ACTIVE_LEVEL=$<IF:$<CONFIG:Debug>,SPDLOG_LEVEL_TRACE,SPDLOG_LEVEL_OFF>)

target_include_directories(skeleton
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(skeleton PRIVATE libskeleton)

cpplint_add_target(skeleton ROOT skeleton/include)
