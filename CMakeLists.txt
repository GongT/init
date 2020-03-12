cmake_minimum_required(VERSION 3.0.0)

message("CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "-pedantic")
# set(CMAKE_RULE_MESSAGES ON)
# set(CMAKE_VERBOSE_MAKEFILE ON)

add_definitions(-D_POSIX_C_SOURCE -D_GNU_SOURCE)
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3")
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")

project(init VERSION 0.1.0)

file(GLOB F src/*.cpp)
add_executable(init ${F})
target_link_libraries(init pthread)
target_compile_options(init PRIVATE -Werror -Wall -Wextra)

set_property(TARGET init PROPERTY C_STANDARD 11)
set_property(TARGET init PROPERTY C_STANDARD_REQUIRED True)

# file(GLOB F2 test-program/*.c)
# add_executable("test-program" ${F2})
