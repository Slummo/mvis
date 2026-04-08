# glad
add_library(glad STATIC
    glad/src/glad.c
)
target_compile_options(glad PRIVATE
    -Wno-sign-conversion
)
# target_compile_definitions(glad PUBLIC _GNU_SOURCE)
target_include_directories(glad PUBLIC glad/include)
add_library(mvis::glad ALIAS glad)