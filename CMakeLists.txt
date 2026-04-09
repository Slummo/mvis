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
pkg_check_modules(PORTAUDIO REQUIRED IMPORTED_TARGET portaudio-2.0)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenMP REQUIRED)

# glad
add_subdirectory(extern)

# window
add_library(window STATIC src/window.c)
target_include_directories(window PUBLIC include)
target_link_libraries(window PRIVATE mvis::glad glfw)
add_library(mvis::window ALIAS window)

# audio
add_library(audio STATIC src/audio/stream.c src/audio/fft.c)
target_include_directories(audio PUBLIC include)
target_include_directories(audio PRIVATE ${PORTAUDIO_INCLUDE_DIRS})
target_compile_definitions(audio PRIVATE _GNU_SOURCE)
target_link_libraries(audio PRIVATE m PkgConfig::SNDFILE ${PORTAUDIO_LIBRARIES} OpenMP::OpenMP_C)
add_library(mvis::audio ALIAS audio)

# render
add_library(render STATIC src/render/io.c src/render/visualizer.c)
target_include_directories(render PUBLIC include)
target_compile_definitions(render PRIVATE _GNU_SOURCE)
target_link_libraries(render PRIVATE m mvis::glad OpenGL::GL)
add_library(mvis::render ALIAS render)

# main
add_executable(main src/main.c)
target_compile_definitions(main PRIVATE _GNU_SOURCE)
target_link_libraries(main PRIVATE mvis::window mvis::audio mvis::render)