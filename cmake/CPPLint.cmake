find_program(CPPLINT cpplint REQUIRED)

function(cpplint_add_target TARGET)
    cmake_parse_arguments(ARG "" "ROOT" "" ${ARGN})

    get_target_property(_sources ${TARGET} SOURCES)
    if(NOT _sources)
        return()
    endif()

    set(_lint_sources "")
    foreach(_src IN LISTS _sources)
        if(NOT IS_ABSOLUTE "${_src}")
            set(_src "${CMAKE_CURRENT_SOURCE_DIR}/${_src}")
        endif()
        if(_src MATCHES "\\.(cpp|h|hpp)$")
            list(APPEND _lint_sources "${_src}")
        endif()
    endforeach()

    if(NOT _lint_sources)
        return()
    endif()

    set(_cpplint_args --linelength=80)
    if(ARG_ROOT)
        set(_cpplint_args ${_cpplint_args} --root=${ARG_ROOT})
    endif()

    add_custom_target(${TARGET}_cpplint
        COMMAND ${CPPLINT} ${_cpplint_args} ${_lint_sources}
        COMMENT "Linting ${TARGET} with cpplint..."
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )

    add_dependencies(${TARGET} ${TARGET}_cpplint)
endfunction()
