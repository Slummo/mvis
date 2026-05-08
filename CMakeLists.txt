cmake_minimum_required(VERSION 4.0)
project(mvis VERSION 1.0.0 LANGUAGES C)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/common.cmake)

set_default_build_type()
set_output_directories()

add_subdirectory(extern)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SNDFILE REQUIRED IMPORTED_TARGET sndfile)
pkg_check_modules(PORTAUDIO REQUIRED IMPORTED_TARGET portaudio-2.0)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenMP REQUIRED)

# Executable
add_executable(${PROJECT_NAME} "")
target_sources(${PROJECT_NAME} PRIVATE
    src/fft/fft.c
    src/io/io.c
    src/window/window.c
    src/engine/stream.c
    src/engine/player.c
    src/engine/visualizer.c
    src/engine/engine.c
    src/main.c
)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PORTAUDIO_INCLUDE_DIRS}
)
target_compile_features(${PROJECT_NAME} PRIVATE c_std_11)
apply_compile_options(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PRIVATE m OpenMP::OpenMP_C mvis::glad OpenGL::GL glfw PkgConfig::SNDFILE ${PORTAUDIO_LIBRARIES})