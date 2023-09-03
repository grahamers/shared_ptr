cmake_minimum_required(VERSION 3.22.1)

project(shared_ptr_impl_test)

if (MSVC)
    # warning level 4
    add_compile_options(/W4)
else()
    # additional warnings
    add_compile_options(-Wall -O0 -g -Wextra -Wpedantic)
endif()

include_directories(detail impl)

file(GLOB SOURCES "tests/test_main.cpp")

set(EXECUTABLE_OUTPUT_PATH "bin")

message("*\n*\n****** linux build option (gcc) currently set to -Wall -O0 -g -Wextra -Wpedantic, change as appropriate  ****** \n*\n*")

add_executable(shared_ptr_test ${SOURCES})
