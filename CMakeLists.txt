cmake_minimum_required(VERSION 4.0)
project(fft VERSION 0.1.0 LANGUAGES C)
add_compile_options(
     -Wall
     -Wextra
     -Werror
     -Wsign-conversion
     -Wno-unused
     -O3
     -march=native
)

find_package(OpenMP REQUIRED)
add_library(fft STATIC
    src/fft.c
)
target_compile_definitions(fft PUBLIC _GNU_SOURCE)
target_include_directories(fft PUBLIC include)
target_link_libraries(fft PUBLIC m OpenMP::OpenMP_C)

add_executable(main src/main.c)
target_link_libraries(main PRIVATE fft)