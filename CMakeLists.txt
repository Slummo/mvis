cmake_minimum_required(VERSION 4.0)
project(mvis VERSION 0.1.0 LANGUAGES C)
add_compile_options(
     -Wall
     -Wextra
     -Werror
     -Wsign-conversion
     -Wno-unused
     # -O3
     # -march=native
     -O0
     -g
)
# Strip the project source directory from __FILE__ macros
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-fmacro-prefix-map=${CMAKE_SOURCE_DIR}/=)
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(SNDFILE REQUIRED IMPORTED_TARGET sndfile)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenMP REQUIRED)

# glad
add_subdirectory(extern)

# track
add_library(track STATIC src/track.c)
target_include_directories(track PUBLIC include)
target_link_libraries(track PRIVATE PkgConfig::SNDFILE)
add_library(mvis::track ALIAS track)

# window
add_library(window STATIC src/window.c)
target_include_directories(window PUBLIC include)
target_link_libraries(window PRIVATE mvis::glad glfw OpenGL)
add_library(mvis::window ALIAS window)

# fft
add_library(fft STATIC src/fft.c)
target_include_directories(fft PUBLIC include)
target_link_libraries(fft PRIVATE OpenMP::OpenMP_C)
add_library(mvis::fft ALIAS fft)

# io
add_library(io STATIC src/io.c)
target_include_directories(io PUBLIC include)
add_library(mvis::io ALIAS io)

# render
add_library(render STATIC src/render.c)
target_include_directories(render PUBLIC include)
target_compile_definitions(render PRIVATE _GNU_SOURCE)
target_link_libraries(render PRIVATE mvis::glad OpenGL::GL mvis::track mvis::io m)
add_library(mvis::render ALIAS render)

add_executable(main src/main.c)
target_link_libraries(main PRIVATE mvis::track mvis::window mvis::fft mvis::render)