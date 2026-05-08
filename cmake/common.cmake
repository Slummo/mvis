macro(set_default_build_type)
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
    endif()
endmacro()

macro(set_output_directories)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endmacro()

function(apply_compile_options TARGET_NAME)
    if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${TARGET_NAME} PRIVATE -fmacro-prefix-map=${CMAKE_SOURCE_DIR}/=)
    endif()

    target_compile_options(${TARGET_NAME} PRIVATE
        -Wall
        -Wextra
        -Werror
        -Wsign-conversion
        -Wno-unused

        # Debug flags
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Debug>:-O0>
        $<$<CONFIG:Debug>:-fsanitize=address,leak,undefined>
        $<$<CONFIG:Debug>:-fno-omit-frame-pointer>
        $<$<CONFIG:Debug>:-DDEBUG>

        # Release flags
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Release>:-march=native>
        $<$<CONFIG:Release>:-DNDEBUG>
    )

    target_link_options(${TARGET_NAME} PRIVATE
        $<$<CONFIG:Debug>:-fsanitize=address,leak,undefined>
    )
endfunction()