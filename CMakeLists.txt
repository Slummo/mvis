cmake_minimum_required(VERSION 4.0)
project(mvis VERSION 3.0.0 LANGUAGES C)
add_compile_options(
    -Wall
    -Wextra
    -Werror
    -Wsign-conversion
    -Wno-unused
    -O3
    -march=native
    # -O0
    # -g
)

# Strip the project source directory from __FILE__ macros
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-fmacro-prefix-map=${CMAKE_SOURCE_DIR}/=)
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(SNDFILE REQUIRED IMPORTED_TARGET sndfile)
pkg_check_modules(PORTAUDIO REQUIRED IMPORTED_TARGET portaudio-2.0)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenMP REQUIRED)

# fft
add_library(fft STATIC src/fft/fft.c)
target_include_directories(fft PRIVATE include)
target_compile_definitions(fft PUBLIC _GNU_SOURCE)
target_link_libraries(fft PUBLIC m PRIVATE OpenMP::OpenMP_C)
add_library(mvis::fft ALIAS fft)

# io
add_library(io STATIC src/io/io.c)
target_include_directories(io PRIVATE include)
add_library(mvis::io ALIAS io)

# glad
add_subdirectory(extern)

# window
add_library(window STATIC src/window/window.c)
target_include_directories(window PRIVATE include)
target_link_libraries(window PUBLIC mvis::glad OpenGL::GL PRIVATE glfw)
add_library(mvis::window ALIAS window)

# engine
add_library(engine STATIC src/engine/stream.c src/engine/player.c src/engine/visualizer.c src/engine/engine.c)
target_include_directories(engine PUBLIC include PRIVATE src ${PORTAUDIO_INCLUDE_DIRS})
target_link_libraries(engine PRIVATE PkgConfig::SNDFILE ${PORTAUDIO_LIBRARIES} mvis::fft mvis::io mvis::window)
add_library(mvis::engine ALIAS engine)

# main
add_executable(main src/main.c)
target_link_libraries(main PRIVATE mvis::engine)